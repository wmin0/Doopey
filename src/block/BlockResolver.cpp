#include "block/BlockResolver.h"

#include "block/BlockManager.h"
#include "block/BlockLocationAttr.h"
#include "common/Doopey.h"
#include "common/Config.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>

using namespace Doopey;

using std::pair;
using std::ifstream;
using std::string;
using std::stringstream;

BlockResolver::BlockResolver(
  const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager), _localDir("."), _cacheRemoteSize(0), _localMax(0) {
  if (NULL != config) {
    string tmp = config->getValue("BlockDir");
    if ("" != tmp) {
      _localDir = DoopeyRoot + tmp;
    }
  }
  // some setup
  loadLocalIDs();
}

BlockResolver::~BlockResolver() {
}

BlockID BlockResolver::newLocalID() {
  return buildBlockID(_manager->getMachineID(), ++_localMax);
}


void BlockResolver::loadLocalIDs() {
  DIR* dir;
  struct dirent* file;
  dir = opendir(_localDir.data());
  unsigned long long int tmp;
  // clear
  // TODO: save in encode64?
  MachineID machine = _manager->getMachineID();
  _localIDs.clear();
  while (NULL != (file = readdir(dir))) {
    if (0 == strcmp(file->d_name, ".") ||
        0 == strcmp(file->d_name, "..")) {
      continue;
    }
    if (16 != strlen(file->d_name)) {
      log->warning("detect illegel file %s\n", file->d_name);
      continue;
    }
    if (0 == (tmp = strtoull(file->d_name, NULL, 16))) {
      log->warning("detect illegel file %s\n", file->d_name);
      continue;
    }
    log->info("found local block: %s\n", file->d_name);
    _localIDs.insert(tmp);
    // TODO: if change to new Machine ID...
    MachineID mid = getMachineIDFromBlockID(tmp);
    if (machine == mid) {
      LocalBlockID bid = getLocalIDFromBlockID(tmp);
      if (bid > _localMax) {
        _localMax = bid;
      }
    }
  }
  closedir(dir);
}

void BlockResolver::addLocalID(BlockID id) {
  // TODO: check local file exist
  stringstream ss("");
  ss << _localDir << "/" << id;
  ifstream file(ss.str(), ifstream::in);
  if (file.good()) {
    _localIDs.insert(id);
  } else {
    log->error("add invalid id\n");
  }
  file.close();
}

void BlockResolver::cleanCache() {
  // TODO: maybe use LRU?
}

BlockLocationAttrSPtr BlockResolver::askBlock(BlockID id) {
  if (_localIDs.end() != _localIDs.find(id)) {
    // machineID = 0 is local
    return BlockLocationAttrSPtr(new BlockLocationAttr(id, 0, 0));
  }
  map<BlockID, BlockLocationAttrSPtr>::iterator it = _remoteIDs.find(id);
  if (_remoteIDs.end() != it) {
    it->second->ts = time(0);
    return it->second;
  }
  BlockLocationAttrSPtr tmp = askRemoteBlock(id);
  if (NULL != tmp) {
    if (_cacheRemoteSize <= _remoteIDs.size()) {
      cleanCache();
    }
    _remoteIDs.insert(pair<BlockID, BlockLocationAttrSPtr>(id, tmp));
  }
  return tmp;
}

BlockLocationAttrSPtr BlockResolver::askRemoteBlock(BlockID id) {
  // TODO: _manager->_router
  return BlockLocationAttrSPtr(NULL);
}

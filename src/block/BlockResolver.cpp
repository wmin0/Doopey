#include "block/BlockResolver.h"

#include "block/BlockManager.h"
#include "block/BlockLocationAttr.h"
#include "common/Doopey.h"
#include "config/Config.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>

using namespace Doopey;

using std::pair;
using std::ifstream;

BlockResolver::BlockResolver(const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager), _localDir("."), _cacheRemoteSize(0) {
  // some setup
  loadLocalIDs();
}

BlockResolver::~BlockResolver() {
}

void BlockResolver::loadLocalIDs() {
  DIR* dir;
  struct dirent* file;
  dir = opendir(_localDir.data());
  unsigned long long int tmp;
  // clear
  _localIDs.clear();
  while (NULL != (file = readdir(dir))) {
    if (0 == strcmp(file->d_name, ".") ||
        0 == strcmp(file->d_name, "..")) {
      continue;
    }
    if (0 == (tmp = strtoull(file->d_name, NULL, 10))) {
      log.warning("detect illegel file %s\n", file->d_name);
    } else {
      _localIDs.insert(tmp);
    }
  }
  closedir(dir);
}

void BlockResolver::addLocalID(BlockID id) {
  // TODO: check local file exist
  char buf[1024] = {0};
  sprintf(buf, "%s/%lld", _localDir.data(), id);
  ifstream file(buf, ifstream::in);
  if (file.good()) {
    _localIDs.insert(id);
  } else {
    log.error("add invalid id\n");
  }
  file.close();
}

void BlockResolver::cleanCache() {
  // TODO: maybe use LRU?
}

BlockLoactionAttrSPtr BlockResolver::askBlock(BlockID id) {
  if (_localIDs.end() != _localIDs.find(id)) {
    // machineID = 0 is local
    return BlockLoactionAttrSPtr(new BlockLoactionAttr(id, 0, 0));
  }
  map<BlockID, BlockLoactionAttrSPtr>::iterator it = _remoteIDs.find(id);
  if (_remoteIDs.end() != it) {
    it->second->ts = time(0);
    return it->second;
  }
  BlockLoactionAttrSPtr tmp = askRemoteBlock(id);
  if (NULL != tmp) {
    if (_cacheRemoteSize <= _remoteIDs.size()) {
      cleanCache();
    }
    _remoteIDs.insert(pair<BlockID, BlockLoactionAttrSPtr>(id, tmp));
  }
  return tmp;
}

BlockLoactionAttrSPtr BlockResolver::askRemoteBlock(BlockID id) {
  // TODO: _manager->_router
  return BlockLoactionAttrSPtr(NULL);
}

#include "block/BlockResolver.h"

#include "block/BlockManager.h"
#include "block/BlockLocationAttr.h"
#include "common/Doopey.h"
#include "common/Config.h"
#include "common/Message.h"
#include "network/Router.h"

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
#include <unistd.h>

using namespace Doopey;

using std::pair;
using std::ifstream;
using std::string;
using std::stringstream;

const int BlockResolver::waitRemote = 5;
const size_t BlockResolver::remoteSizeMax = 1000;

BlockResolver::BlockResolver(
  const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager), _cacheRemoteSize(0),
  _localMax(0), _health(true) {
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
  const char* localDir = _manager->getLocalDir().data();
  dir = opendir(localDir);
  if (NULL == dir) {
    log->error("Open BlockDir fail: %s\n", localDir);
    _health = false;
    return;
  }
  unsigned long long int tmp;
  // clear
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
    forceAddLocalID(tmp);
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
  ss << _manager->getLocalDir() << "/" << id;
  ifstream file(ss.str(), ifstream::in);
  if (file.good()) {
    forceAddLocalID(id);
  } else {
    log->error("add invalid id\n");
  }
  file.close();
}

void BlockResolver::forceAddLocalID(BlockID id) {
  BlockLocationAttrSPtr attr(new BlockLocationAttr(id, 0, BS_Available));
  attr->addMachine(_manager->getMachineID());
  _localIDs[id] = attr;
}

void BlockResolver::forceAddRemoteID(MachineID m, BlockID id) {
  BlockLocationAttrSPtr attr(new BlockLocationAttr(id, 0, BS_Available));
  attr->addMachine(id);
  _remoteIDs[id] = attr;
}

void BlockResolver::cleanCache() {
  // TODO: maybe use LRU?
}

BlockLocationAttrSPtr BlockResolver::askBlock(BlockID id) {
  if (_localIDs.end() != _localIDs.find(id)) {
    // machineID = 0 is local
    return BlockLocationAttrSPtr(new BlockLocationAttr(id, 0, BS_Available));
  }
  BlockMap::iterator it = _remoteIDs.find(id);
  if (_remoteIDs.end() != it) {
    it->second->ts = time(0);
    return it->second;
  }
  BlockLocationAttrSPtr tmp = askRemoteBlock(id);
  return tmp;
}

BlockLocationAttrSPtr BlockResolver::askRemoteBlock(BlockID id) {
  // TODO: check protocol
  MessageSPtr msg(new Message(MT_Block, MC_RequestBlockLocation));
  MachineID m = _manager->getMachineID();
  msg->addData((unsigned char*)&m, 0, sizeof(MachineID));
  msg->addData((unsigned char*)&id, sizeof(MachineID), sizeof(BlockID));
  _manager->getRouter()->broadcast(msg);
  sleep(waitRemote);
  BlockMap::iterator it = _remoteIDs.find(id);
  if (_remoteIDs.end() != it) {
    it->second->ts = time(0);
    return it->second;
  }
  return BlockLocationAttrSPtr(NULL);
}

bool BlockResolver::handleRequestBlockLocation(const MessageSPtr& msg) {
  // TOOD: check protocol
  MachineID m;
  BlockID id;
  memcpy(&m, msg->getData().data(), sizeof(MachineID));
  memcpy(&id, msg->getData().data() + sizeof(MachineID), sizeof(BlockID));
  MessageSPtr ack(new Message(MT_Block, MC_RequestBlockLocationACK));
  BlockMap::iterator it = _localIDs.find(id);
  if (_localIDs.end() != it) {
    ack->addData((unsigned char*)&id, 0, sizeof(BlockID));
    size_t off = sizeof(BlockID);
    for (size_t i = 0; i < it->second->machine.size(); ++i) {
      MachineID mid = it->second->machine[i];
      ack->addData((unsigned char*)&mid, off, sizeof(MachineID));
      off += sizeof(MachineID);
    }
  }
  _manager->getRouter()->sendTo(m, msg);
  return true;
}

bool BlockResolver::handleRequestBlockLocationACK(const MessageSPtr& msg) {
  if (0 == msg->getData().size()) {
    return true;
  }
  MachineID m;
  BlockID id;
  memcpy(&id, msg->getData().data(), sizeof(BlockID));
  memcpy(&m, msg->getData().data() + sizeof(BlockID), sizeof(MachineID));
  size_t off = sizeof(BlockID) + sizeof(MachineID);
  // check
  BlockMap::iterator it = _remoteIDs.find(id);
  if (_remoteIDs.end() != it) {
    it->second->ts = time(0);
    // TODO: check repeated
    it->second->addMachine(m);
  } else {
    if (_remoteIDs.size() >= remoteSizeMax) {
      cleanCache();
    }
    forceAddRemoteID(m, id);
    it = _remoteIDs.find(id);
  }
  while (off < msg->getData().size()) {
    memcpy(&m, msg->getData().data() + off, sizeof(MachineID));
    off += sizeof(MachineID);
    it->second->addMachine(m);
  }
  return true;
}

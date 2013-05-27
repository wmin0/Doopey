#include "block/BlockResolver.h"

#include "block/BlockManager.h"
#include "block/BlockLocationAttr.h"
#include "common/Doopey.h"
#include "common/Config.h"
#include "common/Message.h"
#include "common/Socket.h"
#include "network/Router.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>

using namespace Doopey;

using std::pair;
using std::ifstream;
using std::string;
using std::stringstream;

const int BlockResolver::waitRemote = 5;
const size_t BlockResolver::remoteSizeMax = 1000;
// TODO: adjust after test
const time_t BlockResolver::checkReplicaInterval = 0;

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
  return Doopey::buildBlockID(_manager->getMachineID(), ++_localMax);
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
    MachineID mid = Doopey::getMachineIDFromBlockID(tmp);
    if (machine == mid) {
      LocalBlockID bid = Doopey::getLocalIDFromBlockID(tmp);
      //log->debug("split id %d\n", bid);
      if (bid > _localMax) {
        _localMax = bid;
        log->debug("set _localMax: %d\n", _localMax);
      }
    }
  }
  closedir(dir);
}

void BlockResolver::addLocalID(BlockID id) {
  // TODO: check local file exist
  string path = _manager->convertBlockIDToPath(id);
  ifstream file(path, ifstream::in);
  if (file.good()) {
    forceAddLocalID(id);
  } else {
    log->error("add invalid id %lld\n", id);
  }
  file.close();
}

void BlockResolver::forceAddLocalID(BlockID id) {
  BlockLocationAttrSPtr attr(new BlockLocationAttr(id, 0, BS_Available));
  attr->addMachine(_manager->getMachineID());
  log->debug("add local ID: %lld\n", id);
  _localIDs[id] = attr;
  _remoteIDs.erase(id);
}

void BlockResolver::forceAddRemoteID(MachineID m, BlockID id) {
  BlockLocationAttrSPtr attr(new BlockLocationAttr(id, 0, BS_Available));
  attr->addMachine(m);
  _remoteIDs[id] = attr;
}

void BlockResolver::removeRemoteID(BlockID id) {
  _remoteIDs.erase(id);
}

void BlockResolver::removeLocalID(BlockID id) {
  log->info("delete Block %lld\n", id);
  _localIDs.erase(id);
  string path = _manager->convertBlockIDToPath(id);
  unlink(path.data());
}

void BlockResolver::cleanCache() {
  // TODO: maybe use LRU?

  // flush five
  for (int i = 0; i < 5; ++i) {
    _remoteIDs.erase(_remoteIDs.begin());
  }
}

bool BlockResolver::checkReplica(BlockLocationAttrSPtr& attr) {
  log->debug("CheckReplica: %s\n", attr->toString().data());
  bool requestReplica = Block::blockReplica > attr->machine.size();

  MessageSPtr msg(new Message(MT_Block, MC_CheckBlockAlive));
  msg->addData((unsigned char*)&(attr->block), 0, sizeof(BlockID));
  for (size_t i = attr->machine.size() - 1;
       i >= 0 && i < attr->machine.size();
       --i) {
    // TODO: send all replica ?
    bool succ = true;
    do {
      SocketSPtr sock = _manager->getRouter()->sendTo(attr->machine[i], msg);
      if (NULL == sock) {
        succ = false;
        break;
      }
      MessageSPtr ack = sock->receive();
      if (NULL == ack) {
        succ = false;
        break;
      }
      bool result = false;
      memcpy(&result, ack->getData().data(), sizeof(bool));
      if (!result) {
        succ = false;
        break;
      }
    } while (0);
    if (!succ) {
      log->warning("check replica fail: %lld->%d\n", attr->block, attr->machine[i]);
      requestReplica = true;
      attr->machine.erase(attr->machine.begin() + i);
    }
  }
  if (0 == attr->machine.size()) {
    _remoteIDs.erase(attr->block);
    return false;
  }
  if (requestReplica) {
    log->debug("request replica: %d->%lld\n", attr->machine[0], attr->block);
    MessageSPtr req(new Message(MT_Block, MC_DoReplica));
    req->addData((unsigned char*)&(attr->block), 0, sizeof(BlockID));
    size_t off = sizeof(BlockID);
    for (size_t i = 0; i < attr->machine.size(); ++i) {
      req->addData((unsigned char*)&(attr->machine[i]), off, sizeof(MachineID));
      off += sizeof(MachineID);
    }
    _manager->getRouter()->sendTo(attr->machine[0], req);
  }
  return true;
}

MachineID BlockResolver::chooseReplica(const BlockLocationAttrSPtr& attr) {
  int maxmem = 0;
  int mem = 0;
  size_t a = 0;
  MessageSPtr req(new Message(MT_Machine, MC_RequestSysInfoMem));

  for (size_t i = 0; i < attr->machine.size(); ++i) {
    SocketSPtr sock = _manager->getRouter()->sendTo(attr->machine[i], req);
    if (sock == NULL) {
      log->error("connet machine %d error\n", attr->machine[i]);
      continue;
    }

    MessageSPtr ack = sock->receive();
    if (ack == NULL) {
      log->error("lose machine %d 's ack\n", attr->machine[i]);
      continue;
    }

    memcpy(&mem, ack->getData().data(), sizeof(int));
    //cout << "mem: " << mem << endl;

    if (mem > maxmem) {
      maxmem = mem;
      a = i;
    }
  }

  return attr->machine[a];
}

BlockLocationAttrSPtr BlockResolver::askBlock(BlockID id) {
  log->debug("askBlock %lld\n", id);
  if (_localIDs.end() != _localIDs.find(id)) {
    log->debug("find at local\n");
    BlockLocationAttrSPtr ans(new BlockLocationAttr(id, 0, BS_Available));
    ans->addMachine(_manager->getMachineID());
    return ans;
  }
  BlockMap::iterator it = _remoteIDs.find(id);
  time_t now = time(0);
  BlockLocationAttrSPtr tmp;
  if (_remoteIDs.end() != it) {
    log->debug("find cache\n");
    if (checkReplicaInterval <= now - it->second->ts) {
      if (checkReplica(it->second)) {
        it->second->ts = now;
        tmp = it->second;
      } else {
        log->debug("checkReplica fail remote ask\n");
        tmp = askRemoteBlock(id);
      }
    }
  } else {
    log->debug("remote ask\n");
    tmp = askRemoteBlock(id);
  }
  if (NULL != tmp) {
    MachineID mid = chooseReplica(tmp);
    BlockLocationAttrSPtr ans(new BlockLocationAttr(id, 0, BS_Available));
    ans->addMachine(mid);
    return ans;
  }
  log->debug("no found\n");
  return NULL;
}

BlockLocationAttrSPtr BlockResolver::askRemoteBlock(BlockID id) {
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

BlockLocationAttrSPtr BlockResolver::askBlockDetail(BlockID id) {
  BlockLocationAttrSPtr tmp = askBlock(id);
  if (NULL == tmp) {
    return NULL;
  }
  BlockMap::iterator it = _localIDs.find(id);
  if (_localIDs.end() != it) {
    return it->second;
  }
  it = _remoteIDs.find(id);
  return it->second;
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
  _manager->getRouter()->sendTo(m, ack);
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

bool BlockResolver::handleCheckBlockAlive(
  const SocketSPtr& sock, const MessageSPtr& msg) {
  BlockID id;
  memcpy(&id, msg->getData().data(), sizeof(BlockID));
  bool alive = false;
  if (_localIDs.end() != _localIDs.find(id)) {
    alive = true;
  }
  MessageSPtr ack(new Message(MT_Block, MC_BlockACK));
  ack->addData((unsigned char*)&alive, 0, sizeof(bool));
  return sock->send(ack);
}

bool BlockResolver::handleUpdateReplica(const MessageSPtr& msg) {
  BlockID id;
  MachineID mid;
  memcpy(&id, msg->getData().data(), sizeof(BlockID));
  size_t off = sizeof(BlockID);
  vector<MachineID> mids;
  while (off < msg->getData().size()) {
    memcpy(&mid, msg->getData().data() + off, sizeof(MachineID));
    off += sizeof(MachineID);
    mids.push_back(mid);
  }
  // TODO: delete
  stringstream tmp("");
  for (size_t i = 0; i < mids.size(); ++i) {
    tmp << i << " ";
  }
  log->debug("update replica: %lld->%s\n", id, tmp.str().data());
  // update local
  BlockMap::iterator it = _localIDs.find(id);
  if (_localIDs.end() != it) {
    it->second->machine = mids;
  }
  // update remote
  it = _remoteIDs.find(id);
  if (_remoteIDs.end() != it) {
    it->second->machine = mids;
    it->second->ts = time(0);
  }
  return true;
}

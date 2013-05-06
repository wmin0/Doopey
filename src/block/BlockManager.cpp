#include "block/BlockManager.h"

#include "block/Block.h"
#include "block/BlockLoader.h"
#include "block/BlockResolver.h"
#include "block/BlockSaver.h"
#include "block/BlockLocationAttr.h"
//#include "block/BlockUpdater.h"
#include "block/DataBlock.h"
#include "block/MetaBlock.h"
#include "common/Config.h"
#include "common/Message.h"
#include "common/Thread.h"
#include "machine/Server.h"

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace Doopey;
using namespace std;

const MachineID& BlockManager::getMachineID() const {
  return _server->getMachineID();
}

const RouterSPtr& BlockManager::getRouter() const {
  return _server->getRouter();
}

BlockManager::BlockManager(const Server* server, const ConfigSPtr& config):
  _server(server), _localDir(".") {
  if (NULL != config) {
    string tmp = config->getValue("BlockDir");
    if ("" != tmp) {
      _localDir = DoopeyRoot + tmp;
    } else {
      log->warning("Use Default BlockDir .\n");
    }
  } else {
    log->warning("Use Default BlockResolver .\n");
  }
  _resolver.reset(new BlockResolver(this, config));
  _loader.reset(new BlockLoader(this, config));
  _saver.reset(new BlockSaver(this, config));
}

BlockManager::~BlockManager() {
}

MetaBlockSPtr BlockManager::newMeta() {
  return _loader->newMeta();
}

DataBlockSPtr BlockManager::newData() {
  return _loader->newData();
}

MetaBlockSPtr BlockManager::getMeta(BlockID id) {
  BlockLocationAttrSPtr attr = _resolver->askBlock(id);
  if (NULL == attr) {
    log->warning("no found meta block: %lld\n", id);
    return NULL;
  }
  return _loader->loadMeta(attr);
}

DataBlockSPtr BlockManager::getData(BlockID id) {
  BlockLocationAttrSPtr attr = _resolver->askBlock(id);
  if (NULL == attr) {
    log->warning("no found data block: %lld\n", id);
    return NULL;
  }

  return _loader->loadData(attr);
}

BlockID BlockManager::saveBlock(const BlockSPtr& block) {
  BlockID oldID = block->getID();
  BlockID newID = _saver->saveBlock(block);
  if (0 != oldID && newID != oldID) {
    // TODO: updater? invalid?
  }
  return newID;
}

string BlockManager::convertBlockIDToPath(const BlockID& id) const {
  stringstream ss("");
  ss << _localDir << "/"
     << internal << setfill('0') << uppercase
     << hex << setw(16) << id;
  return ss.str();
}

bool BlockManager::isHealth() const {
  return _resolver->isHealth();
}

void BlockManager::request(const MessageSPtr& msg, const SocketSPtr& sock) {
  switch (msg->getCmd()) {
    case MC_RequestBlockLocation:
      _resolver->handleRequestBlockLocation(msg);
      break;
    case MC_RequestBlockLocationACK:
      _resolver->handleRequestBlockLocationACK(msg);
      break;
    case MC_RequestBlockData:
      _loader->handleRequestBlockData(sock, msg);
      break;
    case MC_CheckBlockAlive:
      _resolver->handleCheckBlockAlive(sock, msg);
      break;
    case MC_DoReplica:
      handleDoReplica(msg);
      break;
    case MC_UpdateReplica:
      _resolver->handleUpdateReplica(msg);
    default:
      break;
  }
}

bool BlockManager::handleDoReplica(const MessageSPtr& msg) {
  // TODO:
  return true;
}


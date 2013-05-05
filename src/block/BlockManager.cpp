#include "block/BlockManager.h"

#include "block/Block.h"
#include "block/BlockLoader.h"
#include "block/BlockResolver.h"
#include "block/BlockSaver.h"
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

BlockID BlockManager::saveBlock(const BlockSPtr& block) {
  BlockID oldID = block->getID();
  BlockID newID = _saver->saveBlock(block);
  if (0 != oldID && newID != oldID) {
    // TODO: updater? invalid?
  }
  return newID;
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
    default:
      break;
  }
}

string BlockManager::convertBlockIDToPath(const BlockID& id) const {
  stringstream ss("");
  ss << _localDir << "/"
     << internal << setfill('0') << uppercase
     << hex << setw(16) << id;
  return ss.str();
}

#include "block/BlockManager.h"

#include "block/Block.h"
#include "block/BlockLoader.h"
#include "block/BlockResolver.h"
#include "block/BlockSaver.h"
//#include "block/BlockUpdater.h"
#include "block/DataBlock.h"
#include "block/MetaBlock.h"
#include "common/Message.h"
#include "common/Thread.h"
#include "machine/Server.h"

using namespace Doopey;

const uint32_t BlockManager::checkReplicaInterval = 3600;

const MachineID& BlockManager::getMachineID() const {
  return _server->getMachineID();
}

const RouterSPtr& BlockManager::getRouter() const {
  return _server->getRouter();
}

BlockManager::BlockManager(const Server* server, const ConfigSPtr& config): _server(server), _run(0) {
  _resolver.reset(new BlockResolver(this, config));
  _loader.reset(new BlockLoader(this, config));
  _saver.reset(new BlockSaver(this, config));
  _thread.reset(new Thread(threadFunc, threadStop));
}

BlockManager::~BlockManager() {
  stop();
}

bool BlockManager::start() {
  log->debug("BlockManager Thread start!!\n");
  return _thread->start(this);
}

bool BlockManager::stop() {
  log->debug("BlockManager Thread stop!!\n");
  return _thread->stop(this);
}

void BlockManager::threadFunc(void* obj) {
  BlockManager* manager = (BlockManager*)obj;
  manager->_run = true;
  manager->mainLoop();
}

void BlockManager::threadStop(void* obj) {
  BlockManager* manager = (BlockManager*)obj;
  manager->_run = false;
}

void BlockManager::mainLoop() {
  while (_run) {
    sleep(BlockManager::checkReplicaInterval);
  }
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
    default:
      break;
  }
}

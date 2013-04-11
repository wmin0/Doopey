#include "block/BlockManager.h"

#include "block/Block.h"
#include "block/BlockLoader.h"
#include "block/BlockResolver.h"
#include "block/BlockSaver.h"
//#include "block/BlockUpdater.h"
#include "block/DataBlock.h"
#include "block/MetaBlock.h"
#include "machine/Server.h"

using namespace Doopey;

typedef shared_ptr<MetaBlock> MetaBlockSPtr;
typedef shared_ptr<DataBlock> DataBlockSPtr;
typedef shared_ptr<Block> BlockSPtr;

MachineID BlockManager::getMachineID() const {
  return _server->getMachineID();
}

BlockManager::BlockManager(const Server* server, const ConfigSPtr& config): _server(server) {
  _resolver.reset(new BlockResolver(this, config));
  _loader.reset(new BlockLoader(this, config));
  _saver.reset(new BlockSaver(this, config));
}

BlockManager::~BlockManager() {}

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

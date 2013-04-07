#include "block/BlockManager.h"

#include "block/Block.h"
#include "block/BlockLoader.h"
#include "block/BlockResolver.h"
#include "block/BlockSaver.h"
//#include "block/BlockUpdater.h"
#include "block/DataBlock.h"
#include "block/MetaBlock.h"

using namespace Doopey;

BlockManager::BlockManager(const ConfigSPtr& config) {
  _resolver.reset(new BlockResolver(this, config));
  _loader.reset(new BlockLoader(this, config));
  _saver.reset(new BlockSaver(this, config));

}

BlockManager::~BlockManager() {}




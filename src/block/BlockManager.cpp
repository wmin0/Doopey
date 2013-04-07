#include "block/BlockManager.h"

#include "block/Block.h"
#include "block/BlockLoader.h"
#include "block/BlockResolver.h"
#include "block/BlockSaver.h"
//#include "block/BlockUpdater.h"
#include "block/DataBlock.h"
#include "block/DataDelegate.h"
#include "block/MetaBlock.h"
#include "block/MetaDelegate.h"
#include "block/OperationDelegate.h"

using namespace Doopey;

BlockManager::BlockManager(const ConfigSPtr& config) {
  _resolver.reset(new BlockResolver(this, config));
  _loader.reset(new BlockLoader(this, config));
  _saver.reset(new BlockSaver(this, config));

  MetaBlock::delegate.reset(new MetaDelegate());
  DataBlock::delegate.reset(new DataDelegate());
}

BlockManager::~BlockManager() {}




#include "block/BlockManager.h"

#include "block/BlockLoader.h"
#include "block/BlockResolver.h"
//#include "block/BlockSaver.h"
//#include "block/BlockUpdater.h"

using namespace Doopey;

BlockManager::BlockManager(const ConfigSPtr& config) {
  _resolver.reset(new BlockResolver(this, config));
  _loader.reset(new BlockLoader(this, config));
}

BlockManager::~BlockManager() {}




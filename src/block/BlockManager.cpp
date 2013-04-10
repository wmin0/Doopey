#include "block/BlockManager.h"

#include "block/Block.h"
#include "block/BlockLoader.h"
#include "block/BlockResolver.h"
#include "block/BlockSaver.h"
//#include "block/BlockUpdater.h"
#include "block/DataBlock.h"
#include "block/MetaBlock.h"

using namespace Doopey;

typedef shared_ptr<MetaBlock> MetaBlockSPtr;
typedef shared_ptr<DataBlock> DataBlockSPtr;
typedef shared_ptr<Block> BlockSPtr;

BlockManager::BlockManager(const ConfigSPtr& config) {
  _resolver.reset(new BlockResolver(this, config));
  _loader.reset(new BlockLoader(this, config));
  _saver.reset(new BlockSaver(this, config));

}

BlockManager::~BlockManager() {}

MetaBlockSPtr BlockManager::newMeta()
{
  return _loader->newMeta();
}

DataBlockSPtr BlockManager::newData()
{
  return _loader->newData();
}

BlockID saveBlock(const BlockSPtr& block)
{
  return 0; 
}

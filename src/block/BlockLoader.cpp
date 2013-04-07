#include "block/BlockLoader.h"

#include "block/Block.h"
#include "block/BlockLocationAttr.h"
#include "block/DataBlock.h"
#include "block/MetaBlock.h"

using namespace Doopey;

BlockLoader::BlockLoader(const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager) {
}

BlockLoader::~BlockLoader() {
}

BlockSPtr BlockLoader::loadData(const BlockLocationAttrSPtr& attr) const {
  unsigned char* data = getData(attr);
  if (NULL != data) {
    return BlockSPtr(new DataBlock(data, attr->block));
  }
  return BlockSPtr(NULL);
}

BlockSPtr BlockLoader::loadMeta(const BlockLocationAttrSPtr& attr) const {
  unsigned char* data = getData(attr);
  if (NULL != data) {
    return BlockSPtr(new MetaBlock(data, attr->block));
  }
  return BlockSPtr(NULL);
}

unsigned char* BlockLoader::getData(const BlockLocationAttrSPtr& attr) const {
  return NULL;
}

BlockSPtr BlockLoader::newData() const {
  // TODO: check allocate success
  // NOTE: allocate id by Saver, because Saver know about local information
  return BlockSPtr(new MetaBlock(allocateMem(), 0));
}
BlockSPtr BlockLoader::newMeta() const {
  // TODO: check allocate success
  return BlockSPtr(new DataBlock(allocateMem(), 0));
}

unsigned char* BlockLoader::allocateMem() const {
  return NULL;
}

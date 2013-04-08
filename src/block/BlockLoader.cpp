#include "block/BlockLoader.h"

#include "block/Block.h"
#include "block/BlockLocationAttr.h"
#include "block/DataBlock.h"
#include "block/MetaBlock.h"

using namespace Doopey;

typedef shared_ptr<Config> ConfigSPtr;
typedef shared_ptr<DataBlock> DataBlockSPtr;
typedef shared_ptr<MetaBlock> MetaBlockSPtr;

BlockLoader::BlockLoader(const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager) {
}

BlockLoader::~BlockLoader() {
}

DataBlockSPtr BlockLoader::loadData(const BlockLocationAttrSPtr& attr) const {
  unsigned char* data = getData(attr);
  if (NULL != data) {
    return DataBlockSPtr(new DataBlock(data, attr->block));
  }
  return DataBlockSPtr(NULL);
}

MetaBlockSPtr BlockLoader::loadMeta(const BlockLocationAttrSPtr& attr) const {
  unsigned char* data = getData(attr);
  if (NULL != data) {
    return MetaBlockSPtr(new MetaBlock(data, attr->block));
  }
  return MetaBlockSPtr(NULL);
}

unsigned char* BlockLoader::getData(const BlockLocationAttrSPtr& attr) const {
  return NULL;
}

DataBlockSPtr BlockLoader::newData() const {
  // TODO: check allocate success
  // NOTE: allocate id by Saver, because Saver know about local information
  return DataBlockSPtr(new DataBlock(allocateMem(), 0));
}
MetaBlockSPtr BlockLoader::newMeta() const {
  // TODO: check allocate success
  return MetaBlockSPtr(new MetaBlock(allocateMem(), 0));
}

unsigned char* BlockLoader::allocateMem() const {
  return NULL;
}

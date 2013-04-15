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
  // NOTE: allocate id by Saver, because Saver know about local information
  // NOTE: don't use vector here for controlling memory.
  unsigned char* mem = allocateMem();
  if (NULL == mem) {
    return DataBlockSPtr(NULL);
  }
  return DataBlockSPtr(new DataBlock(mem, 0));
}
MetaBlockSPtr BlockLoader::newMeta() const {
  unsigned char* mem = allocateMem();
  if (NULL == mem) {
    return MetaBlockSPtr(NULL);
  }
  return MetaBlockSPtr(new MetaBlock(mem, 0));
}

unsigned char* BlockLoader::allocateMem() const {
  unsigned char* mem = NULL;
  try {
    mem = new unsigned char[Block::blockSize];
  } catch (...) {
    mem = NULL;
  }
  return mem;
}

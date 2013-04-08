#ifndef _DOOPEY_BLOCK_LOADER_H_
#define _DOOPEY_BLOCK_LOADER_H_

#include <memory>

using std::shared_ptr;

namespace Doopey {

  class BlockLocationAttr;
  class BlockManager;
  class Config;
  class DataBlock;
  class MetaBlock;

  typedef shared_ptr<DataBlock> DataBlockSPtr;
  typedef shared_ptr<MetaBlock> MetaBlockSPtr;

  class BlockLoader {
    typedef shared_ptr<BlockLocationAttr> BlockLocationAttrSPtr;
    typedef shared_ptr<Config> ConfigSPtr;
    public:
      BlockLoader(const BlockManager* manager, const ConfigSPtr& config);
      ~BlockLoader();
      DataBlockSPtr loadBlock(const BlockLocationAttrSPtr& attr) const;
      MetaBlockSPtr loadBlock(const BlockLocationAttrSPtr& attr) const;
      DataBlockSPtr newBlock() const;
      MetaBlockSPtr newBlock() const;

    private:
      unsigned char* getData(const BlockLocationAttrSPtr& attr) const;
      unsigned char* allocateMem() const;
      const BlockManager* _manager;

  }; // class BlockLoader

};  // namespace Doopey

#endif

#ifndef _DOOPEY_BLOCK_LOADER_H_
#define _DOOPEY_BLOCK_LOADER_H_

#include <memory>

using std::shared_ptr;

namespace Doopey {

  class Block;
  class BlockLocationAttr;
  class BlockManager;
  class Config;

  typedef shared_ptr<Block> BlockSPtr;

  class BlockLoader {
    typedef shared_ptr<BlockLocationAttr> BlockLocationAttrSPtr;
    typedef shared_ptr<Config> ConfigSPtr;
    public:
      BlockLoader(const BlockManager* manager, const ConfigSPtr& config);
      ~BlockLoader();
      BlockSPtr loadData(const BlockLocationAttrSPtr& attr) const;
      BlockSPtr loadMeta(const BlockLocationAttrSPtr& attr) const;

    private:
      unsigned char* getData(const BlockLocationAttrSPtr& attr) const;
      const BlockManager* _manager;

  }; // class BlockLoader

};  // namespace Doopey

#endif

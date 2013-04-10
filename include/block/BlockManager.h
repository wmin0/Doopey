#ifndef _DOOPEY_BLOCK_MANAGER_H_
#define _DOOPEY_BLOCK_MANAGER_H_

#include "common/Doopey.h"

#include <cstdint>
#include <memory>

using std::shared_ptr;

namespace Doopey {

  class Router;
  class Block;
  class BlockLoader;
  class BlockResolver;
  class BlockSaver;
  class BlockUpdater;
  class Config;
  class DataBlock;
  class MetaBlock;

  class BlockManager {
    typedef shared_ptr<Config> ConfigSPtr;
    typedef shared_ptr<Block> BlockSPtr;
    typedef shared_ptr<Router> RouterSPtr;
    typedef shared_ptr<BlockLoader> BlockLoaderSPtr;
    typedef shared_ptr<BlockResolver> BlockResolverSPtr;
    typedef shared_ptr<BlockSaver> BlockSaverSPtr;
    typedef shared_ptr<BlockUpdater> BlockUpdaterSPtr;
    typedef shared_ptr<DataBlock> DataBlockSPtr;
    typedef shared_ptr<MetaBlock> MetaBlockSPtr;

    public:
      BlockManager(const ConfigSPtr& config);
      ~BlockManager();

      void setRouter(const RouterSPtr& router) { _router = router; }

      // request handle interface
      MetaBlockSPtr getMeta(uint64_t id) { return MetaBlockSPtr(NULL); }
      DataBlockSPtr getData(uint64_t id) { return DataBlockSPtr(NULL); }

      MetaBlockSPtr newMeta();// { return MetaBlockSPtr(NULL); }
      DataBlockSPtr newData();// { return DataBlockSPtr(NULL); }

      BlockID saveBlock(const BlockSPtr& block);// { return 0; }

    private:
      RouterSPtr _router;

      BlockLoaderSPtr _loader;
      BlockResolverSPtr _resolver;
      BlockSaverSPtr _saver;
      BlockUpdaterSPtr _updater;

    friend class BlockResolver;
  }; // class BlockManager

};  // namespace Doopey

#endif

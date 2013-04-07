#ifndef _DOOPEY_BLOCK_RESOLVER_H_
#define _DOOPEY_BLOCK_RESOLVER_H_

#include "common/Doopey.h"

#include <map>
#include <memory>
#include <set>
#include <string>

using std::map;
using std::set;
using std::shared_ptr;
using std::string;

namespace Doopey {

  class Config;
  class BlockLocationAttr;
  class BlockManager;

  typedef shared_ptr<BlockLocationAttr> BlockLocationAttrSPtr;

  class BlockResolver {
    typedef shared_ptr<Config> ConfigSPtr;
    public:
      BlockResolver(const BlockManager* manager, const ConfigSPtr& config);
      ~BlockResolver();

      BlockLocationAttrSPtr askBlock(BlockID id);
      void addLocalID(BlockID id);
      //void removeID(BlockID id);

    private:
      void loadLocalIDs();
      void cleanCache();
      BlockLocationAttrSPtr askRemoteBlock(BlockID id);

    private:
      const BlockManager* _manager;

      string _localDir;
      size_t _cacheRemoteSize;

      set<BlockID> _localIDs;
      map<BlockID, BlockLocationAttrSPtr> _remoteIDs;

  }; // class BlockResolver

};  // namespace Doopey

#endif

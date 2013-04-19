#ifndef _DOOPEY_BLOCK_RESOLVER_H_
#define _DOOPEY_BLOCK_RESOLVER_H_

#include "common/Doopey.h"

#include <map>
#include <set>
#include <string>

using std::map;
using std::set;
using std::string;

namespace Doopey {

  class BlockResolver {
    public:
      BlockResolver(const BlockManager* manager, const ConfigSPtr& config);
      ~BlockResolver();

      BlockLocationAttrSPtr askBlock(BlockID id);
      BlockID newLocalID();
      void addLocalID(BlockID id);
      //void removeID(BlockID id);

      bool isHealth() const { return _health; }

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
      LocalBlockID _localMax;

      bool _health;
  }; // class BlockResolver

};  // namespace Doopey

#endif

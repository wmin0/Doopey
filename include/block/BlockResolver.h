#ifndef _DOOPEY_BLOCK_RESOLVER_H_
#define _DOOPEY_BLOCK_RESOLVER_H_

#include "common/Doopey.h"

#include <map>
#include <set>
#include <string>

using std::map;
using std::set;
using std::string;
using std::pair;

namespace Doopey {

  class BlockResolver {
    typedef map<BlockID, BlockLocationAttrSPtr> BlockMap;
    typedef pair<BlockID, BlockLocationAttrSPtr> BlockPair;
    public:
      BlockResolver(const BlockManager* manager, const ConfigSPtr& config);
      ~BlockResolver();

      BlockLocationAttrSPtr askBlock(BlockID id);
      BlockID newLocalID();
      void addLocalID(BlockID id);
      //void removeID(BlockID id);

      bool isHealth() const { return _health; }

      bool handleRequestBlockLocation(const MessageSPtr& msg);
      bool handleRequestBlockLocationACK(const MessageSPtr& msg);
    private:
      void loadLocalIDs();
      void cleanCache();
      BlockLocationAttrSPtr askRemoteBlock(BlockID id);

    private:
      const BlockManager* _manager;

      string _localDir;
      size_t _cacheRemoteSize;

      set<BlockID> _localIDs;
      BlockMap _remoteIDs;
      LocalBlockID _localMax;

      bool _health;

      static const int waitRemote;
      static const size_t remoteSizeMax;
  }; // class BlockResolver

};  // namespace Doopey

#endif

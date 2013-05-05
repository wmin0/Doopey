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
      bool handleCheckBlockAlive(const SocketSPtr& sock, const MessageSPtr& msg);
      bool handleUpdateReplica(const MessageSPtr& msg);
    private:
      void loadLocalIDs();
      void cleanCache();
      void forceAddLocalID(BlockID id);
      void forceAddRemoteID(MachineID m, BlockID id);
      BlockLocationAttrSPtr askRemoteBlock(BlockID id);
      bool checkReplica(BlockLocationAttrSPtr& attr);
      MachineID chooseReplica(const BlockLocationAttrSPtr& attr);
    private:
      const BlockManager* _manager;

      size_t _cacheRemoteSize;

      BlockMap _localIDs;
      BlockMap _remoteIDs;
      LocalBlockID _localMax;

      bool _health;

      static const int waitRemote;
      static const size_t remoteSizeMax;
      static const time_t checkReplicaInterval;
  }; // class BlockResolver

};  // namespace Doopey

#endif

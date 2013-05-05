#ifndef _DOOPEY_BLOCK_MANAGER_H_
#define _DOOPEY_BLOCK_MANAGER_H_

#include "common/Doopey.h"

#include <cstdint>
#include <string>

using std::string;

namespace Doopey {

  class BlockManager {
    public:
      BlockManager(const Server* server, const ConfigSPtr& config);
      ~BlockManager();
      // request handle interface
      MetaBlockSPtr getMeta(uint64_t id) { return MetaBlockSPtr(NULL); }
      DataBlockSPtr getData(uint64_t id) { return DataBlockSPtr(NULL); }

      MetaBlockSPtr newMeta();
      DataBlockSPtr newData();

      BlockID saveBlock(const BlockSPtr& block);

      void request(const MessageSPtr& msg, const SocketSPtr& sock);

      const BlockLoaderSPtr& getBlockLoader() const { return _loader; }
      const BlockResolverSPtr& getBlockResolver() const { return _resolver; }
      const BlockSaverSPtr& getBlockSaver() const { return _saver; }
      const BlockUpdaterSPtr& getBlockUpdater() const { return _updater; }

      const RouterSPtr& getRouter() const;

      const MachineID& getMachineID() const;
      const string& getLocalDir() const { return _localDir; }

      bool isHealth() const;

    // util
    public:
      string convertBlockIDToPath(const BlockID& id) const;

    private:

      const Server* _server;

      BlockLoaderSPtr _loader;
      BlockResolverSPtr _resolver;
      BlockSaverSPtr _saver;
      BlockUpdaterSPtr _updater;

      string _localDir;

  }; // class BlockManager

};  // namespace Doopey

#endif

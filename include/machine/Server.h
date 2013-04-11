#ifndef _DOOPEY_SERVER_H_
#define _DOOPEY_SERVER_H_

#include "common/Doopey.h"

#include <memory>
#include <pthread.h>

using std::shared_ptr;

namespace Doopey {

  class BlockManager;
  class Dispatcher;
  class FileManager;
  class Router;
  class SectionCollection;

  class Server {
    typedef shared_ptr<SectionCollection> SectionCollectionSPtr;
    typedef shared_ptr<BlockManager> BlockManagerSPtr;
    typedef shared_ptr<Dispatcher> DispatcherSPtr;
    typedef shared_ptr<Router> RouterSPtr;
    typedef shared_ptr<FileManager> FileManagerSPtr;

    public:
      Server(const char* configPath);
      ~Server();
      bool start();

      const BlockManagerSPtr getBlockManager() const { return _blockManager; }
      const RouterSPtr getRouter() const { return _router; }
      const FileManagerSPtr getFileManager() const { return _fileManager; }
      MachineID getMachineID() const { return _machineID; }

    private:
      static void handleTERM(int sig);
      static void handleSREQ(int sig);

      void attachSignal();
      void detachSignal();
      void serve();

    private:
      static Server* _this;

      MachineID _machineID;

      SectionCollectionSPtr _sectionCollection;
      BlockManagerSPtr _blockManager;
      DispatcherSPtr _dispatcher;
      RouterSPtr _router;
      FileManagerSPtr _fileManager;
      pthread_t _thread;
  }; // class Server

};  // namespace Doopey

#endif

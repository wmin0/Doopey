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

  class Server {
    typedef shared_ptr<BlockManager> BlockManagerSPtr;
    typedef shared_ptr<Dispatcher> DispatcherSPtr;
    typedef shared_ptr<Router> RouterSPtr;
    typedef shared_ptr<FileManager> FileManagerSPtr;

    public:
      Server(const SectionCollectionSPtr& section);
      ~Server();
      bool start();

      const BlockManagerSPtr& getBlockManager() const { return _blockManager; }
      const RouterSPtr& getRouter() const { return _router; }
      const FileManagerSPtr& getFileManager() const { return _fileManager; }
      const MachineID& getMachineID() const { return _machineID; }

      void serve();
    private:
      static void handleTERM(int sig);
      static void handleINT(int sig);
      static void signalStop();

      void attachSignal();
      void detachSignal();

    private:
      static Server* _this;

      MachineID _machineID;

      SectionCollectionSPtr _sectionCollection;
      BlockManagerSPtr _blockManager;
      DispatcherSPtr _dispatcher;
      RouterSPtr _router;
      FileManagerSPtr _fileManager;

      // wait lock
      pthread_mutex_t _mutex;
  }; // class Server

};  // namespace Doopey

#endif

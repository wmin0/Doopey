#ifndef _DOOPEY_SERVER_H_
#define _DOOPEY_SERVER_H_

#include <memory>
#include <pthread.h>

using std::shared_ptr;

namespace Doopey {

  class BlockManager;
  class Dispather;
  class FileManager;
  class Router;
  class SectionCollection;

  class Server {
    typedef shared_ptr<SectionCollection> SectionCollectionSPtr;
    typedef shared_ptr<BlockManager> BlockManagerSPtr;
    typedef shared_ptr<Dispather> DispatherSPtr;
    typedef shared_ptr<Router> RouterSPtr;
    typedef shared_ptr<FileManager> FileManagerSPtr;

    public:
      Server(const char* configPath);
      ~Server();
      bool start();

    private:
      void attachSignal();
      void detachSignal();
      void serve();

      SectionCollectionSPtr _sectionCollection;
      BlockManagerSPtr _blockManager;
      DispatherSPtr _dispather;
      RouterSPtr _router;
      FileManagerSPtr _fileManager;
      pthread_t _thread;

      static Server* _this;
      static void handleTERM(int sig);
      static void handleSREQ(int sig);

  }; // class Server

};  // namespace Doopey

#endif

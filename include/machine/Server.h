#ifndef _DOOPEY_SERVER_H_
#define _DOOPEY_SERVER_H_

#include <memory>

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
      SectionCollectionSPtr _sectionCollection;
      BlockManagerSPtr _blockManager;
      DispatherSPtr _dispather;
      RouterSPtr _router;
      FileManagerSPtr _fileManager;

  }; // class Server

};  // namespace Doopey

#endif

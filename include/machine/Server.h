#ifndef _DOOPEY_SERVER_H_
#define _DOOPEY_SERVER_H_

namespace Doopey {

  class SectionCollection;
  class BlockManager;
  class Dispather;
  class Router;
  class FileManager;

  class Server {

    public:
      Server(const char* configPath);
      ~Server();
      bool start();

    private:
      SectionCollection* _sectionCollection;
      BlockManager* _blockManager;
      Dispather* _dispather;
      Router* _router;
      FileManager* _fileManager;

  }; // class Server

};  // namespace Doopey

#endif

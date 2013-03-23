#ifndef _DOOPEY_SERVER_H_
#define _DOOPEY_SERVER_H_

namespace Doopey {

  class Config;
  class BlockManager;
  class Dispather;
  class Router;
  class FileManager;
  class Logger;

  class Server {

    public:
      Server();
      ~Server();
      bool start();

    private:
      Logger* _logger;
      Config* _config;
      BlockManager* _blockManager;
      Dispather* _dispather;
      Router* _router;
      FileManager* _fileManager;

  }; // class Server

};  // namespace Doopey

#endif

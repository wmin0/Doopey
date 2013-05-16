#ifndef _DOOPEY_FILE_MANAGER_H_
#define _DOOPEY_FILE_MANAGER_H_

#include <cstdint>
#include <memory>
#include <map>
#include <string>

#include "common/Doopey.h"
#include "common/Socket.h"

using std::shared_ptr;
using std::string;
using std::map;

namespace Doopey{

  class Server;
  class MetaDecoder;
  class FileUploader;
  class Config;
  class BlockManager;
  class FileTree;

  class FileManager{

    public:
      FileManager(const Server* server, const ConfigSPtr& config);
      ~FileManager();

      //this function is used to tell which work is requested in msg 
      void receiveQuest(const MessageSPtr& msg, const SocketSPtr& socket);

    private:
      //three function belonging is used to handle different msg
      //which will be catch by receiveQuest()
      bool handleUpload(SocketSPtr socket); 
      bool handleGetFile(SocketSPtr socket);
      bool handleList(SocketSPtr socket);

      //check the msg type is for FileManager 
      bool checkMsg(const MessageSPtr& msg) const;

      MetaDecoderSPtr _decoder;
      FileUploaderSPtr _uploader;
      FileTreeSPtr _fileMap;

      const Server* _server;
  };//class FileManager

} //namespace Doopey

#endif

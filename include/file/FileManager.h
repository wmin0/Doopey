#ifndef _DOOPEY_FILE_MANAGER_H_
#define _DOOPEY_FILE_MANAGER_H_

#include <cstdint>
#include <memory>
#include <map>
#include <string>

#include "network/Socket.h"

using std::shared_ptr;
using std::string;
using std::map;

namespace Doopey{

  class Server;
  class MetaDecoder;
  class FileUploader;
  class Config;
  class BlockManager;

  class FileManager{
    typedef shared_ptr<Config> ConfigSPtr;
    typedef shared_ptr<MetaDecoder> MetaDecoderSPtr;
    typedef shared_ptr<FileUploader> FileUploaderSPtr;

    public:
      FileManager(const Server* server, const ConfigSPtr& config);
      ~FileManager();

      //TODO: design the interface of upload file;
      //Thought: first message: tell server the socket used to transmit file
      //         second step: receive entire file, then
      //                      1.split file into different blocks
      //                      2.create meta block(need to design the meta block
      bool uploadFile(SocketSPtr socket); 
      bool getFile(string path) const;

    private:
      MetaDecoderSPtr _decoder;
      FileUploaderSPtr _uploader;

      const Server* _server;
      map<string, uint64_t> _fileTable;
  };//class FileManager

} //namespace Doopey

#endif

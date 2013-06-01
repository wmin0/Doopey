#ifndef _DOOPEY_FILE_UPLOADER_H_
#define _DOOPEY_FILE_UPLOADER_H_

#include <cstdint>
#include <string>
#include <memory>

#include "common/Doopey.h"
#include "common/Socket.h"
#include "common/Message.h"
#include "block/BlockManager.h"
#include "file/FileManager.h"
#include "network/Router.h"

using std::shared_ptr;
using std::string;

namespace Doopey{

  class MetaBlock;
  class DataBlock;
  class Message;

  class FileUploader{

    friend class FileManager;

    public:
      ~FileUploader();
      MetaBlockSPtr receiveFile(SocketSPtr socket);
      void setBlockManager(const BlockManagerSPtr blockManager);
      void setFileTree(FileTreeSPtr tree);
      void cleanData(MetaBlockSPtr meta);
    private:
      FileUploader();
      bool setupMeta(MetaBlockSPtr meta, const MessageSPtr msg);
      bool setupData(MetaBlockSPtr meta, const MessageSPtr msg);

      void returnACK(SocketSPtr socket);
      void returnError(SocketSPtr socket);
      void returnError(SocketSPtr socket, MessageCmd cmd);

      BlockManagerSPtr _blockManager;
      FileTreeSPtr _fileTree;
  };//class FileUploader

}//namespace Doopey

#endif

#ifndef _DOOPEY_FILE_UPLOADER_H_
#define _DOOPEY_FILE_UPLOADER_H_

#include <cstdint>
#include <string>
#include <memory>

#include "common/Doopey.h"
#include "common/Socket.h"
#include "block/BlockManager.h"

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
      bool receiveFile(SocketSPtr socket);
      void setBlockManager(const BlockManagerSPtr blockManager);
    private:
      FileUploader();
      bool setupMeta(MetaBlockSPtr meta, const MessageSPtr msg);

      BlockManagerSPtr _blockManager;
  };//class FileUploader

}//namespace Doopey

#endif

#ifndef _DOOPEY_FILE_UPLOADER_H_
#define _DOOPEY_FILE_UPLOADER_H_

#include <cstdint>
#include <string>
#include <memory>

#include "network/Socket.h"
#include "block/BlockManager.h"

using std::shared_ptr;
using std::string;

namespace Doopey{

  class MetaBlock;
  class DataBlock;

  class FileUploader{

    typedef shared_ptr<MetaBlock> MetaBlockSPtr;
    typedef shared_ptr<BlockManager> BlockManagerSPtr;

    friend class FileManager;

    public:
      ~FileUploader();
      bool receiveFile(Socket socket);
      void setBlockManager(const BlockManagerSPtr blockManager);
    private:
      FileUploader();
      BlockManagerSPtr _blockManager;
  };//class FileUploader

}//namespace Doopey

#endif

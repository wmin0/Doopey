#ifndef _DOOPEY_FILE_UPLOADER_H_
#define _DOOPEY_FILE_UPLOADER_H_

#include <cstdint>
#include <string>
#include <memory>

using std::shared_ptr;
using std::string;

namespace Doopey{

  class MetaBlock;
  class DataBlock;

  class FileUploader{

    typedef shared_ptr<MetaBlock> MetaBlockSPtr;

    friend class FileManager;

    public:
      ~FileUploader();
      bool receiveFile(MetaBlockSPtr meta, uint64_t port, string IP);
      //parameter with path used for test in single machine
      bool receiveFile(MetaBlockSPtr meta, string path);
    private:
      FileUploader();
  };//class FileUploader

}//namespace Doopey

#endif

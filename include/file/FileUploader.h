#ifndef _DOOPEY_FILE_UPLOADER_H_
#define _DOOPEY_FILE_UPLOADER_H_

namespace Doopey{

  class MetaBlock;
  class DataBlock;

  class FileUploader{

    typedef shared_ptr<MetaBlock> MetaBlockSPtr;

    friend class FileManager;

    public:
      ~FileUploader();
      bool receiveFile(MetaBlockSPtr meta, uint64_t port, string IP);

    private:
      FileUploader();
  }

}

#endif

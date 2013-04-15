#include "file/FileUploader.h"
#include "common/Doopey.h"

using namespace Doopey;

typedef shared_ptr<BlockManager> BlockManagerSPtr;

FileUploader::FileUploader()
{
  _blockManager = NULL;
}

FileUploader::~FileUploader()
{
}

bool FileUploader::receiveFile(Socket socket)
{
  if(_blockManager == NULL)
  {
    log.error("FileUploader Error: haven't initialize block manager");
    return false;
  }
  //fill name field in meta
  
  //1.receive file and split file
  //2.make new data block to save segment and add segment ID into metaBlock

  return true;
}

void FileUploader::setBlockManager(const BlockManagerSPtr blockManager)
{
  _blockManager = blockManager;
}

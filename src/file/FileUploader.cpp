#include "file/FileUploader.h"

using namespace Doopey;

FileUploader::FileUploader()
{
}

FileUploader::~FileUploader()
{
}

bool FileUploader::receiveFile(MetaBlockSPtr meta, uint64_t port, string IP)
{
  return true;
}

bool FileUploader::receiveFile(MetaBlockSPtr meta, string path)
{
  //fill name field in meta
  
  //1.receive file and split file
  //2.make new data block to save segment and add segment ID into metaBlock

  return true;
}



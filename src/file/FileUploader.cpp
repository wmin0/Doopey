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
  return true;
}



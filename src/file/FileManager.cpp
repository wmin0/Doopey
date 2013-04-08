#include "file/FileManager.h"
#include "file/MetaDecoder.h"
#include "file/FileUploader.h"

using namespace Doopey;

typedef shared_ptr<MetaBlock> MetaBlockSPtr;

FileManager::FileManager(const ConfigSPtr& config){
  _decoder.reset(new MetaDecoder());
  _uploader.reset(new FileUploader());
}

FileManager::~FileManager()
{

}

bool FileManager::uploadFile(uint64_t port, string IP, string filename, string pathRemote)
{
  MetaBlockSPtr meta;

  bool result = _uploader->receiveFile(meta, port, IP);


  return result;
}

bool FileManager::uploadFile(string pathUpload, string pathRemote)
{
  //create meta
  MetaBlockSPtr meta;

  //transmit meta to FileUploader and receive and split file
  bool result = _uploader->receiveFile(meta, pathUpload);
  //send meta to block manager to get meta's ID

  //add file into map
  //_fileTable[pathRemote, meta.Delegate

  //broadcast that here have a new file

  return result;
}



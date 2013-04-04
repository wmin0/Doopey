#include "file/FileManager.h"
#include "file/MetaDecoder.h"
#include "file/MetaCreator.h"
#include "file/FileUploader.h"

using namespace Doopey;

FileManager::FileManager(const ConfigSPtr& config){
  _decoder.reset(new MetaDecoder());
  _creator.reset(new MetaCreator());
  _uploader.reset(new FileUploader());
}

FileManager::~FileManager()
{

}

bool FileManager::uploadFile(uint64_t port, string IP, string filename)
{
  return true;
}

bool FileManager::uploadFile(string path)
{
  return true;
}



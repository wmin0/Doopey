#include "file/FileManager.h"
#include "file/MetaDecoder.h"
#include "file/FileUploader.h"
#include "block/BlockManager.h"
#include "block/Block.h"
#include "block/MetaBlock.h"
#include "common/Doopey.h"
#include "machine/Server.h"

using namespace Doopey;

typedef shared_ptr<MetaBlock> MetaBlockSPtr;
typedef shared_ptr<BlockManager> BlockManagerSPtr;
typedef shared_ptr<Block> BlockSPtr;

FileManager::FileManager(const Server* server, const ConfigSPtr& config):_server(server){
  _decoder.reset(new MetaDecoder());
  _uploader.reset(new FileUploader());
}

FileManager::~FileManager()
{

}

bool FileManager::uploadFile(Socket socket)
{
  //let uploader to create meta, receive file and create data block
  _uploader->setBlockManager(_server->getBlockManager());
  bool result = _uploader->receiveFile(socket);

  //add file into map

  //broadcast that here have a new file

  return result;
}

bool FileManager::getFile(string path) const
{
  return true;
}

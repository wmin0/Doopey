#include "file/FileManager.h"
#include "file/MetaDecoder.h"
#include "file/FileUploader.h"
#include "block/BlockManager.h"
#include "block/Block.h"
#include "block/MetaBlock.h"

using namespace Doopey;

typedef shared_ptr<MetaBlock> MetaBlockSPtr;
typedef shared_ptr<BlockManager> BlockManagerSPtr;
typedef shared_ptr<Block> BlockSPtr;

FileManager::FileManager(const ConfigSPtr& config, BlockManagerSPtr blockManager){
  _decoder.reset(new MetaDecoder());
  _uploader.reset(new FileUploader());
  _blockManager = blockManager;
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
  MetaBlockSPtr meta = _blockManager->newMeta();

  //transmit meta to FileUploader and receive and split file
  bool result = _uploader->receiveFile(meta, pathUpload);

  //send meta to block manager to get meta's ID
  BlockSPtr block = std::static_pointer_cast<Block>(meta);
  BlockID metaID = _blockManager->saveBlock(block);

  //add file into map
  _fileTable[pathRemote] = metaID;

  //broadcast that here have a new file

  return result;
}



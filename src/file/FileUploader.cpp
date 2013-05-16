#include "common/Doopey.h"
#include "file/FileTree.h"
#include "file/FileUploader.h"
#include "common/Message.h"
#include "block/MetaBlock.h"
#include "block/DataBlock.h"
#include "block/BlockManager.h"

#include <cstring>

using namespace Doopey;

FileUploader::FileUploader()
{
  _blockManager = NULL;
}

FileUploader::~FileUploader()
{
}

bool FileUploader::receiveFile(SocketSPtr socket)
{
  //for sure that blockManager has been set up
  if(_blockManager == NULL)
  {
    log->error("FileUploader Error: haven't initialize block manager");
    return false;
  }

  MetaBlockSPtr meta = _blockManager->newMeta();

  BlockID blockID;
  MessageSPtr msg;
  while(1)
  {
    msg = socket->receive();
    //if msg==NULL means client has disconnected
    //just aboadon the file and return false
    if( NULL==msg){
      //TODO: release the thing has received
      return false;
    }else if( MT_File != msg->getType()){
      log->error("FileUploader Error: error message type of %s in FileUploader\n"
        , enum2String(msg->getType()));
    }

    //decide what to do depend on the command
    switch(msg->getCmd())
    {
      //the msg is about the file meta, like filenam, creat time, etc.
      case MC_UpFileMeta:
        if(setupMeta(meta, msg) == false){
          log->error("Set up meta block error! Msg from %u\n", msg->getSrc());
          return false;//one failure should terminate whole file?
        }
        break;
      //the msg is the file data, just receive and split it into different data 
      //block and add the datablock ID into metablock
      case MC_UpFileData:
        if (setupData(meta, msg) == false){
          log->error("Set up data block error! Msg from %u\n", msg->getSrc());
          return false;//one failure should terminate whole file?
        }
        break;
      //this msg means file has been transmitted successfully just check the 
      //meta ID and add it into file directory structure
      case MC_UpFileEnd:
        blockID = _blockManager->saveBlock(meta);
        _fileTree->addFile(meta->getFileName(), blockID);
        return true;
        break;
      default:
        log->error("FileUploader Error: Unknown command!\n");
        break;
    }
  }

  return true;;
}

void FileUploader::setBlockManager(const BlockManagerSPtr blockManager)
{
  _blockManager = blockManager;
}

void FileUploader::setFileTree(FileTreeSPtr tree)
{
  _fileTree = tree;
}

bool FileUploader::setupMeta(MetaBlockSPtr meta, const MessageSPtr msg)
{
  uint64_t nameLength;
  vector<unsigned char> data;
  string nameStr;
  time_t ctime;

  data = msg->getData();
  memcpy(&nameLength, data.data(), sizeof(nameLength));

  nameStr.resize(nameLength);
  memcpy(&(nameStr[0]), data.data()+sizeof(nameLength), nameLength);
  if( meta->setFileName(nameStr) == false){
    //log->error("Meta Block: set file name failed by %s\n", nameStr.c_str());
    return false;
  }

  memcpy(&ctime, data.data()+sizeof(nameLength)+nameLength, sizeof(time_t));
  if( meta->setCreateTime(ctime) == false ){
    //log->error("Meta Block: set ctime failed by %s\n", nameStr.c_str());
    return false;
  }

  return true;
}

//data of a msg is equal or less than one block size
bool FileUploader::setupData(MetaBlockSPtr meta, const MessageSPtr msg){
  vector<unsigned char> msgData = msg->getData();
  if(msgData.size() > DataBlock::blockSize){
    log->error("data exceed block size! Msg from %u\n", msg->getSrc());
    return false;
  }
  DataBlockSPtr dataBlock = _blockManager->newData();
  dataBlock->copyData(msgData);
  BlockID blockID = _blockManager->saveBlock(dataBlock);
  if(meta->addDataID(blockID) == false)
    return false;
  return true;
}

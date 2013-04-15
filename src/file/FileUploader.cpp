#include "file/FileUploader.h"
#include "network/Message.h"

#define stringiz(arg) #arg //enum to string

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
    log.error("FileUploader Error: haven't initialize block manager");
    return false;
  }

  MetaBlockSPtr meta = _blockManager->newMeta(); 

  MessageSPtr msg;
  while(1)
  {
    msg = socket->receive();
    //if msg==NULL means client has disconnected so just aboadon the file and return false
    if( NULL==msg){
      //TODO: release the thing has received
      return false;
    }else if( MT_File != msg->getType()){
      log.error("FileUploader Error: error message type of %s in FileUploader\n", stringiz(msg->getType()));
    }

    //decide what to do depend on the command
    switch(msg->getCmd())
    {
      //the msg is about the file meta, like filenam, creat time, etc.
      case MC_FileMeta:
        setupMeta(meta, msg);
        break;
      //the msg is the file data, just receive and split it into different data block and 
      //add the datablock ID into metablock
      case MC_FileData:
        break;
      //this msg means file has been transmitted successfully just check the meta ID 
      //and add it into file directory structure
      case MC_FileEnd:
        return true;
        break;
      default:
        log.error("FileUploader Error: Unknown command!\n");
        break;
    }
  }

  return true;
}

void FileUploader::setBlockManager(const BlockManagerSPtr blockManager)
{
  _blockManager = blockManager;
}

bool FileUploader::setupMeta(MetaBlockSPtr meta, const MessageSPtr msg)
{
  

  return true;
}

#include "file/FileManager.h"
#include "file/MetaDecoder.h"
#include "file/FileUploader.h"
#include "block/BlockManager.h"
#include "block/Block.h"
#include "block/MetaBlock.h"
#include "machine/Server.h"
#include "network/Message.h"

using namespace Doopey;

FileManager::FileManager(const Server* server, const ConfigSPtr& config):_server(server){
  _decoder.reset(new MetaDecoder());
  _uploader.reset(new FileUploader());
}

FileManager::~FileManager()
{

}

void FileManager::receiveQuest(SocketSPtr socket){
  MessageSPtr msg = socket->receive();
  if(!checkMsg(msg))
    return;
  switch(msg->getCmd())
  {
    case MC_UpFileStart:
      uploadFile(socket);
      break;
    case MC_RequestFile:
      break;
    case MC_RequestList:
      break;
    default:
      log->warning("FileManager: error message command!\n");
      break;
  }
}

bool FileManager::uploadFile(SocketSPtr socket)
{
  //let uploader to create meta, receive file and create data block
  _uploader->setBlockManager(_server->getBlockManager());
  bool result = _uploader->receiveFile(socket);

  //add file into map

  //broadcast that here have a new file

  return result;
}

bool FileManager::getFile(SocketSPtr socket)
{
  return true;
}

bool FileManager::checkMsg(const MessageSPtr msg) const
{
  if(msg == NULL)
  {
    log->info("FileUploader: Client has disconnect!\n");
    return false;
  }
  if(msg->getType() != MT_File)
  {
    log->info("FileUploader: Error message type!\n");
    return false;
  }
  return true;
}

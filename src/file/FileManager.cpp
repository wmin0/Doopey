#include "file/FileManager.h"
#include "file/MetaDecoder.h"
#include "file/FileUploader.h"
#include "file/FileTree.h"
#include "block/BlockManager.h"
#include "block/Block.h"
#include "block/MetaBlock.h"
#include "machine/Server.h"
#include "common/Message.h"

#include <stdio.h>
#include <string.h>

using namespace Doopey;

FileManager::FileManager(const Server* server, const ConfigSPtr& config):_server(server){
  _decoder.reset(new MetaDecoder());
  _uploader.reset(new FileUploader());
  _fileMap.reset(new FileTree());
}

FileManager::~FileManager()
{

}

void FileManager::receiveQuest(const MessageSPtr& msg, const SocketSPtr& socket){
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
      searchList(socket);
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
  _uploader->setFileTree(_fileMap);
  bool result = _uploader->receiveFile(socket);

  //add file into map

  //broadcast that here have a new file

  return result;
}

bool FileManager::searchList(SocketSPtr socket)
{
  //receive the name of dir which is requested
  uint64_t nameLength;
  MessageSPtr msg = socket->receive();
  memcpy(&nameLength, msg->getData().data(), sizeof(uint64_t));
  string dirName;
  dirName.resize(nameLength);
  memcpy(&(dirName[0]), msg->getData().data() + sizeof(uint64_t), nameLength);

  //get result by _fileMap
  //vector<string> result = _fileMap->getChildren(dirName);
  vector<string> result = _fileMap->getExChildren();

  MessageSPtr reply(new Message(MT_File, MC_RequestList));
  reply->addData((unsigned char*)&result, 0, sizeof(result));
  socket->send(reply);

  //return result to the client which request
  return true;
}

bool FileManager::getFile(SocketSPtr socket)
{
  return true;
}

bool FileManager::checkMsg(const MessageSPtr& msg) const
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

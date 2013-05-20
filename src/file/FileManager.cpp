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
      log->info("FileManager: Start to upload file\n");
      handleUpload(socket);
      break;
    case MC_RequestFile:
      break;
    case MC_RequestList:
      log->info("FileManager: Receive a request of file list\n");
      handleList(socket);
      break;
    default:
      log->warning("FileManager: error message command!\n");
      break;
  }
}

bool FileManager::handleUpload(SocketSPtr socket)
{
  //let uploader to create meta, receive file and create data block
  _uploader->setBlockManager(_server->getBlockManager());
  _uploader->setFileTree(_fileMap);
  log->info("FileManager: Start receiving file\n");
  bool result = _uploader->receiveFile(socket);

  //broadcast that here have a new file

  return result;
}

bool FileManager::handleList(SocketSPtr socket)
{
  //receive the name of dir which is requested
  MessageSPtr msg = socket->receive();
  string dirName;
  unsigned int size = msg->getData().size();
  dirName.resize(size);
  memcpy(&(dirName[0]), msg->getData().data(), size);

  //get result by _fileMap
  vector<string> result = _fileMap->getChildren(dirName);
  //vector<string> result = _fileMap->getExChildren();

  MessageSPtr reply(new Message(MT_File, MC_RequestList));
  for(unsigned int i=0; i<result.size(); i++)
  {
    reply->addData((unsigned char*)result[i].data(), result[i].length());
    reply->addData((const unsigned char*)"\n", 1);
  }
  socket->send(reply);

  //return result to the client which request
  return true;
}

bool FileManager::handleAddDir(SocketSPtr socket)
{
  MessageSPtr msg = socket->receive();
  string dir;
  unsigned int size = msg->getData().size();
  dir.resize(size);
  memcpy(&(dir[0]), msg->getData().data(), size);

  bool result = _fileMap->addDir(dir);
  msg.reset(new Message(MT_File, result?MC_FileACK:MC_FileError));
  socket->send(msg);
  return result;
}

bool FileManager::handleGetFile(SocketSPtr socket)
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

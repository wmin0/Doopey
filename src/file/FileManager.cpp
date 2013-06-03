#include "file/FileManager.h"
#include "file/MetaDecoder.h"
#include "file/FileUploader.h"
#include "file/FileTree.h"
#include "block/BlockManager.h"
#include "block/Block.h"
#include "block/MetaBlock.h"
#include "block/BlockResolver.h"
#include "block/BlockLocationAttr.h"
#include "machine/Server.h"
#include "common/Message.h"
#include "common/Doopey.h"
#include "network/Router.h"

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
      log->info("FileManager: Start to get file\n");
      handleGetFile(socket);
      break;
    case MC_RequestList:
      log->info("FileManager: Receive a request of file list\n");
      handleList(socket);
      break;
    case MC_NewDir:
      log->info("FileManager: Receive a request of adding dir\n");
      handleAddDir(socket);
      break;
    case MC_RmDir:
    case MC_RmFile:
      handleRemove(socket);
      break;
    case MC_BroadcastNewFile:
    case MC_BroadcastNewDir:
    case MC_BroadcastRmFile:
    case MC_BroadcastRmDir:
      handleBroadcast(msg);
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
  MetaBlockSPtr meta = _uploader->receiveFile(socket);

  if(meta==NULL)
    return false;

  //broadcast that here have a new file
  //broadcast format: length of name, name, ID of metablock
  string filename = meta->getFileName();
  uint64_t l=filename.length();
  BlockID id = meta->getID();
  MessageSPtr msg(new Message(MT_File, MC_BroadcastNewFile));
  msg->addData((unsigned char*)&l, sizeof(l));
  msg->addData((unsigned char*)filename.data(), l);
  msg->addData((unsigned char*)&id, sizeof(id));
  (_server->getRouter())->broadcast(msg);

  return true;
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

  log->info("FileManager: Add dir %s\n", dir.data());

  bool success = _fileMap->addDir(dir);
  if(success){
    returnACK(socket);
    log->info("FileManager: Add dir successfully\n");
    msg.reset(new Message(MT_File, MC_BroadcastNewDir));
    msg->addData((unsigned char*)dir.data(), size);
    (_server->getRouter())->broadcast(msg);
  }else{
    log->info("FileManager: Add dir fail\n");
    returnError(socket);
  }
  return success;
}

bool FileManager::handleBroadcast(const MessageSPtr& msg)
{
  bool success;
  string name;
  switch(msg->getCmd())
  {
    case MC_BroadcastNewFile:
      uint64_t l;
      memcpy(&l, msg->getData().data(), sizeof(l));
      name.resize(l);
      memcpy(&(name[0]), msg->getData().data()+sizeof(l), l);
      BlockID meta;
      memcpy(&meta, msg->getData().data()+sizeof(l)+l, sizeof(meta));
      log->info("FileManager: receive a broadcast of adding new file %s, %d\n"
        , name.data(), meta);
      success = _fileMap->addFile(name, meta);
      break;
    case MC_BroadcastNewDir:
      success = _fileMap->addDir(getString(msg));
      break;
    case MC_BroadcastRmFile:
      success = _fileMap->removeFile(getString(msg));
      break;
    case MC_BroadcastRmDir:
      success = _fileMap->removeDir(getString(msg));
      break;
    default:
      log->error("FileManager: Error msg cmd\n");
      return false;
  }
  return success;
}

bool FileManager::handleGetFile(SocketSPtr socket)
{
  MessageSPtr msg = socket->receive();
  string path;
  path.resize(msg->getData().size());
  memcpy(&(path[0]), msg->getData().data(), msg->getData().size());

  log->info("FileManager: receive the request message size is %d\n", msg->getData().size());
  log->info("FileManager: a file request of %s\n", path.data());

  //get meta block from block manager
  const BlockManagerSPtr blockManager = _server->getBlockManager();
  RouterSPtr router = _server->getRouter();
  BlockID metaID = _fileMap->getMetaID(path);
  if(metaID==0)
  {
    returnError(socket);
    return false;
  }
  returnACK(socket);
  MetaBlockSPtr meta = blockManager->getMeta(metaID);

  uint64_t filesize = meta->getFileSize();
  msg.reset(new Message(MT_File, MC_RequestFile));
  msg->addData((unsigned char*)&filesize, sizeof(filesize));
  socket->send(msg);

  //Start transfer location of block to Client
  uint64_t nBlock = meta->getDataBlockNumber();
  msg.reset(new Message(MT_File, MC_RequestFile));
  msg->addData((unsigned char*)&nBlock, sizeof(nBlock));
  socket->send(msg);
  log->info("FileManager: number of block = %d\n", nBlock);

  BlockResolverSPtr resolver = blockManager->getBlockResolver();
  BlockID id;           //variable used in loop
  MachineID mid;
  BlockLocationAttrSPtr location;
  string ip;
  for(uint64_t i=0; i<nBlock; i++)
  {
    id = meta->getDataBlockID(i);
    location = resolver->askBlock(id);
    mid = location->machine[0];
    ip = router->askMachineIP(mid);
    log->info("FileManager: get data block of ID=%u, machineID=%d, locationIP=%s\n", id, mid, ip.data());
    msg.reset(new Message(MT_File, MC_RequestFile));
    msg->addData((unsigned char*)&id, sizeof(id));
    msg->addData((unsigned char*)ip.data(), ip.length());
    socket->send(msg);
  }
  msg.reset(new Message(MT_File, MC_FileACK));
  socket->send(msg);
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

bool FileManager::handleRemove(SocketSPtr socket)
{
  MessageSPtr msg = socket->receive();
  string path = getString(msg);
  log->info("FileManager: receive a request of remove %s\n", path.data());

  BlockID id;
  BlockManagerSPtr _blockManager = _server->getBlockManager();
  MetaBlockSPtr meta;

  switch(msg->getCmd()){
    case MC_RmDir:
      break;
    case MC_RmFile:
      id = _fileMap->getMetaID(path);
      if(id != 0){
        returnACK(socket);
        _fileMap->removeFile(path);
        meta = _blockManager->getMeta(id);
        _uploader->cleanData(meta);
        msg.reset(new Message(MT_File, MC_BroadcastRmFile));
        (_server->getRouter())->broadcast(msg);
      }else{
        returnError(socket);
        return false;
      }
      break;
    default:
      log->error("FileManager: Error cmd in remove\n");
      return false;
      break;
  }

  return true;
}

void FileManager::returnACK(SocketSPtr socket)
{
  MessageSPtr msg(new Message(MT_File, MC_FileACK));
  socket->send(msg);
}

void FileManager::returnError(SocketSPtr socket)
{
  returnError(socket, MC_FileError);
}

void FileManager:returnError(SocketSPtr socket, MessageCmd cmd)
{
  MessageSPtr msg(new Message(MT_File, cmd));
  socket->send(msg);
}

string FileManager::getString(const MessageSPtr& msg) const
{
  string name;
  size_t l=msg->getData().size();
  name.resize(l);
  memcpy(&(name[0]), msg->getData().data(), l);
  return name;
}

string FileManager::snapshot()
{
  string result = _fileMap->getTotalTree();
  return result;
}

void FileManager::loadSnapshot(const string& dump)
{
  log->info("FileManager: get all dump\n%s\n", dump.data());
  string alldump = dump;

  string record;
  string filename;
  string ID;

  while(alldump.find_first_of("\n")!=string::npos)
  {
    record = alldump.substr(0, alldump.find_first_of("\n"));
    filename = record.substr(0, record.find_first_of(":"));
    ID = record.substr(record.find_first_of(":")+1);
    alldump.erase(0, alldump.find_first_of("\n")+1);
    log->info("FileManager: load a record of %s\n", record.data());
    log->info("           : split into %s, %s\n", filename.data(), ID.data());

    if(filename == "/")
      continue;

    BlockID id= stol(ID);
    if(id == 0)
      _fileMap->addDir(filename);
    else
      _fileMap->addFile(filename, id);
  }
  //string working = dump.substr(
}

#include "machine/Server.h"

#include "file/FileManager.h"
#include "block/BlockManager.h"
#include "common/Doopey.h"
#include "common/Config.h"
#include "common/Message.h"
#include "common/SectionCollection.h"
#include "common/Socket.h"
#include "machine/ServerSnapshot.h"
#include "network/Dispatcher.h"
#include "network/Router.h"

// test include
#include "block/Block.h"
#include "block/DataBlock.h"

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <csignal>
#include <pthread.h>
#include <sys/statfs.h>
#include <stdio.h>
#include <iostream>
#include <cstring>

using namespace Doopey;

Server* Server::_this = NULL;

Server::Server(const SectionCollectionSPtr& section):
  _sectionCollection(section), _machineID(0), _machineIDMax(0), _ip(""),
  _useSpaceMax(0), _health(true) {
  attachSignal();
  setupLocalIP();

  pthread_mutex_init(&_mutex, NULL);
  // TODO: snapshot
  loadSnapshot();
  // NOTE: notice initial dependent
  _router.reset(new Router(this, _sectionCollection->getConfig("router")));
  _dispatcher.reset(new Dispatcher(this, _sectionCollection->getConfig("")));
  _blockManager.reset(
    new BlockManager(this, _sectionCollection->getConfig("block")));
<<<<<<< HEAD
  ConfigSPtr config = _sectionCollection->getConfig("global");
  string tmp = config->getValue("UseSpaceMax");
  // TODO: validataion
  _useSpaceMax = strtoull(tmp.data(), NULL, 10);
  if (0 == _useSpaceMax) {
    log->error("Server UseSpaceMax == 0\n");
    _health = false;
  } else {
    log->info("Server UseSpaceMax: %lld\n", _useSpaceMax);
  }
=======
  _fileManager.reset(
    new FileManager(this, _sectionCollection->getConfig("file")));
>>>>>>> ee7c859662e5d83122a83dd4074b254a123930d3
}

Server::~Server() {
  detachSignal();
  // TODO: snapshot
  saveSnapshot();
  pthread_mutex_destroy(&_mutex);
}

void Server::setupLocalIP() {
  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifa=NULL;
  void * tmpAddrPtr=NULL;

  getifaddrs(&ifAddrStruct);

  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
      if (AF_INET == ifa->ifa_addr->sa_family) { // check it is IP4
        // is a valid IP4 Address
        tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        char addressBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
        if (0 == strncmp("eth", ifa->ifa_name, 3)) {
          _ip = addressBuffer;
          break;
        }
      }
  }
  if (ifAddrStruct!=NULL) {
    freeifaddrs(ifAddrStruct);
  }
  log->info("Server IP: %s\n", _ip.data());
  if ("" == _ip) {
    // TODO: init fail tag;
  }
}

bool Server::start() {
  return _health &&
         _blockManager->isHealth() &&
         _dispatcher->start() &&
         _router->start();
}

void Server::serve() {
  pthread_mutex_lock(&_mutex);
  pthread_mutex_lock(&_mutex);
  pthread_mutex_unlock(&_mutex);
}

// signal
void Server::attachSignal() {
  Server::_this = this;
  signal(SIGTERM, Server::handleTERM);
  signal(SIGINT, Server::handleINT);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGUSR1, Server::handleUSR1);
}

void Server::detachSignal() {
  signal(SIGTERM, NULL);
  signal(SIGINT, NULL);
  signal(SIGPIPE, SIG_DFL);
  signal(SIGUSR1, NULL);
  Server::_this = NULL;
}

void Server::handleTERM(int sig) {
  log->info("Server Receive TERM\n");
  signalStop();
}

void Server::handleINT(int sig) {
  log->info("Server Receive INT\n");
  signalStop();
}

void Server::signalStop() {
  pthread_mutex_unlock(&(Server::_this->_mutex));
}

void Server::handleUSR1(int sig) {
  log->info("Server Receive USR1\n");
  // do testing you want
/*
  DataBlockSPtr block = _this->_blockManager->newData();
  log->debug("new block\n");
  BlockID id = _this->_blockManager->saveBlock((BlockSPtr)block);
  log->debug("save new block %d\n", id);
*/
  DataBlockSPtr block = _this->_blockManager->getData(Doopey::buildBlockID(4, 1));
}


// snapshot
void Server::saveSnapshot() {
  ServerSnapshot snapshot;
  snapshot.setMachineID(_machineID);
  if (!snapshot.save()) {
    log->warning("can't save snapshot\n");
  }
}

void Server::loadSnapshot() {
  ServerSnapshot snapshot;
  if (!snapshot.load()) {
    log->warning("can't find snapshot\n");
    return;
  }
  _machineID = snapshot.getMachineID();
}


void Server::request(const MessageSPtr& msg, const SocketSPtr& sock) {
  if (MT_Machine != msg->getType()) {
    return;
  }
  switch (msg->getCmd()) {
    case MC_RequestSysInfoMem:
      getSysInfoFreeMemInKB(sock);
      break;
    case MC_RequestSysInfoDisk:
      getSysInfoAvailDisk(sock);
      break;
    default:
      break;
  }
}


bool Server::getSysInfoFreeMemInKB( const SocketSPtr& sock)
{
  MessageSPtr ack(new Message(MT_Machine, MC_SysInfoACK));

  FILE *meminfo = popen("grep MemFree /proc/meminfo", "r");
  char line[256];
  fgets(line, sizeof(line), meminfo);
  int ram;
  log->debug("get line:%s\n", line);
  sscanf(line, "MemFree: %d kB", &ram);
  pclose(meminfo);

  ack->addData((unsigned char*)&ram, 0, sizeof(uint64_t));
  log->info("Server Free Mem: %d\n", ram);

  if (!sock->send(ack)) {
    log->warning("send SysInfo ack fail\n");
    return false;
  }

  return true;
}


bool Server::getSysInfoAvailDisk(const SocketSPtr& sock)
{
  MessageSPtr ack(new Message(MT_Machine, MC_SysInfoACK));

  FILE *fp;
  // TODO: fix block dir
  fp = popen( "du -s |awk '{print $1}'", "r");
  char line[256];
  fgets(line, sizeof(line), fp);
  pclose(fp);

  uint64_t sizenow = strtoull(line, NULL, 10);
  uint64_t usage = (_useSpaceMax - sizenow);
  ack->addData((unsigned char*)&usage, 0, sizeof(uint64_t));
  log->info("Server Free Space: %d\n", usage);

/*
  string test = "hello world.";
  uint64_t len = test.size();
  ack->addData((unsigned char*)&len, 0, sizeof(uint64_t));
  ack->addData((unsigned char*)test.data(), sizeof(uint64_t), len);
*/

  if (!sock->send(ack)) {
    log->warning("send SysInfo ack fail\n");
    return false;
  }
  return true;
}


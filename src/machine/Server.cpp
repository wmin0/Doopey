#include "machine/Server.h"

#include "block/BlockManager.h"
#include "common/Doopey.h"
#include "common/Config.h"
#include "common/Message.h"
#include "common/SectionCollection.h"
#include "common/Socket.h"
#include "machine/ServerSnapshot.h"
#include "network/Dispatcher.h"
#include "network/Router.h"

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

using namespace Doopey;

Server* Server::_this = NULL;

Server::Server(const SectionCollectionSPtr& section):
  _sectionCollection(section), _machineID(0), _machineIDMax(0), _ip("") {
  attachSignal();
  setupLocalIP();

  pthread_mutex_init(&_mutex, NULL);
  // TODO: snapshot
  loadSnapshot();
  _blockManager.reset(
    new BlockManager(this, _sectionCollection->getConfig("block")));
  _router.reset(new Router(this, _sectionCollection->getConfig("router")));
  _dispatcher.reset(new Dispatcher(this, _sectionCollection->getConfig("")));
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
  return _blockManager->isHealth() &&
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
  signal(SIGUSR1, server::handleUSR1);
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
  log->info("Server Receive USR1"\n);
  // do testing you want
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
   // case MC_RequestSysInfoMem:
   //   getSysInfoFreeMemInKB(sock);
    case MC_RequestSysInfoDisk:
      getSysInfoAvailDisk(sock);
      break;
    default:
      break;
  }
}

/*
int Server::getSysInfoFreeMemInKB()
{
  FILE *meminfo = fopen("/proc/meminfo", "r");
  if(meminfo == NULL) {
    log->waring("file opening error:");
  }
  while(fgets(line, sixeof(line), meminfo))
  {
    int ram;
    if(sscanf(line, "MemFree: $d kB", &ram) == 1)
    {
      fclose(meminfo);
      return ram;
    }
  }
  fclose(meminfo);
  return -1;

}

*/
bool Server::getSysInfoAvailDisk(const SocketSPtr& sock)
{
  MessageSPtr ack(new Message(MT_Machine, MC_SysInfoACK));
  string test = "hello world.";
  uint64_t len = test.size();
  ack->addData((unsigned char*)&len, 0, sizeof(uint64_t));
  ack->addData((unsigned char*)test.data(), sizeof(uint64_t), len);
  if (!sock->send(ack)) {
    log->warning("send SysInfo ack fail\n");
    return false;
  }
  return true;
}


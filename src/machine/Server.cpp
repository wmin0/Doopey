#include "machine/Server.h"

#include "block/BlockManager.h"
#include "common/Doopey.h"
#include "common/Config.h"
#include "common/SectionCollection.h"
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
}

void Server::detachSignal() {
  signal(SIGTERM, NULL);
  signal(SIGINT, NULL);
  signal(SIGPIPE, SIG_DFL);
  Server::_this = NULL;
}

void Server::handleTERM(int sig) {
  log->info("Server Recieve TERM\n");
  signalStop();
}

void Server::handleINT(int sig) {
  log->info("Server Recieve INT\n");
  signalStop();
}

void Server::signalStop() {
  pthread_mutex_unlock(&(Server::_this->_mutex));
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

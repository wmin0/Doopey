#include "machine/Server.h"

#include "block/BlockManager.h"
#include "common/Doopey.h"
#include "config/Config.h"
#include "config/ConfigLoader.h"
#include "config/SectionCollection.h"
#include "network/Router.h"

#include <csignal>
#include <pthread.h>

using namespace Doopey;

Server* Server::_this = NULL;

Server::Server(const char* configPath) {
  attachSignal();
  _thread = pthread_self();

  ConfigLoader loader;
  _sectionCollection.reset(loader.loadConfig(configPath));
  _blockManager.reset(new BlockManager(_sectionCollection->getConfig("")));
  _router.reset(new Router(_sectionCollection->getConfig("")));
}

Server::~Server() {
  detachSignal();
}

bool Server::start() {
  _router->start();
  return true;
}

void Sever::serve() {
}

void Server::attachSignal() {
  Server::_this = this;
  signal(SIGTERM, Server::handleTERM);
  signal(SIGSREQ, Server::handleSREQ);
}

void Server::detachSignal() {
  signal(SIGTERM, NULL);
  signal(SIGSREQ, NULL);
  Server::_this = NULL;
}

void Server::handleTERM(int sig) {
  log.debug("Server Recive TERM\n");
}

void Server::handleSREQ(int sig) {
}


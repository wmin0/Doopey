#include "machine/Server.h"

#include "block/BlockManager.h"
#include "common/Doopey.h"
#include "config/Config.h"
#include "config/ConfigLoader.h"
#include "config/SectionCollection.h"
#include "network/Dispatcher.h"
#include "network/Router.h"

#include <csignal>
#include <pthread.h>

using namespace Doopey;

Server* Server::_this = NULL;

Server::Server(const char* configPath) {
  attachSignal();
  _thread = pthread_self();

  ConfigLoader loader;
  _sectionCollection = loader.loadConfig(configPath);
  _blockManager.reset(new BlockManager(this, _sectionCollection->getConfig("")));
  _router.reset(new Router(this, _sectionCollection->getConfig("")));
  _dispatcher.reset(new Dispatcher(this, _sectionCollection->getConfig("")));
}

Server::~Server() {
  detachSignal();
}

bool Server::start() {
  _router->start();
  _dispatcher->start();
  return true;
}

void Server::serve() {
}

void Server::attachSignal() {
  Server::_this = this;
  signal(SIGTERM, Server::handleTERM);
  signal(SIGSREQ, Server::handleSREQ);
  signal(SIGPIPE, SIG_IGN);
}

void Server::detachSignal() {
  signal(SIGTERM, NULL);
  signal(SIGSREQ, NULL);
  signal(SIGPIPE, SIG_DFL);
  Server::_this = NULL;
}

void Server::handleTERM(int sig) {
  log.debug("Server Recieve TERM\n");
}

void Server::handleSREQ(int sig) {
}


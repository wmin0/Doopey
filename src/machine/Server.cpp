#include "machine/Server.h"

#include "block/BlockManager.h"
#include "common/Doopey.h"
#include "common/Config.h"
#include "common/SectionCollection.h"
#include "network/Dispatcher.h"
#include "network/Router.h"

#include <csignal>
#include <pthread.h>

using namespace Doopey;

Server* Server::_this = NULL;

Server::Server(const SectionCollectionSPtr& section):
  _sectionCollection(section) {
  attachSignal();

  pthread_mutex_init(&_mutex, NULL);

  _blockManager.reset(
    new BlockManager(this, _sectionCollection->getConfig("")));
  _router.reset(new Router(this, _sectionCollection->getConfig("")));
  _dispatcher.reset(new Dispatcher(this, _sectionCollection->getConfig("")));
}

Server::~Server() {
  detachSignal();
  pthread_mutex_destroy(&_mutex);
}

bool Server::start() {
  return _dispatcher->start() && _router->start();
}

void Server::serve() {
  pthread_mutex_lock(&_mutex);
  pthread_mutex_lock(&_mutex);
  pthread_mutex_unlock(&_mutex);
}

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
  log->debug("Server Recieve TERM\n");
  signalStop();
}

void Server::handleINT(int sig) {
  log->debug("Server Recieve INT\n");
  signalStop();
}

void Server::signalStop() {
  pthread_mutex_unlock(&(Server::_this->_mutex));
}

#include "network/Router.h"

#include "common/Config.h"
#include "common/Doopey.h"
#include "common/Thread.h"

using namespace Doopey;

Router* Router::_this = NULL;

Router::Router(const Server* server, const ConfigSPtr& config): _server(server), _run(false) {
  _thread.reset(new Thread(threadFunc, threadStop));
}

Router::~Router() {
  stop();
}

bool Router::start() {
  log->debug("Router Thread start!!\n");
  return _thread->start(this);
}

bool Router::stop() {
  log->debug("Router Thread stop!!\n");
  return _thread->stop(this);
}

void Router::threadFunc(void* obj) {
  log->debug("Router Func!!\n");
  Router* router = (Router*)obj;
  router->_run = true;
  router->mainLoop();
}

void Router::mainLoop() {
  while (_run);
}

void Router::threadStop(void* obj) {
  Router* router = (Router*)obj;
  router->_run = false;
}

void Router::handleRREQ(int sig) {
}

#include "network/Router.h"

#include "config/Config.h"
#include "common/Doopey.h"

#include <pthread.h>
#include <csignal>

using namespace Doopey;

Router* Router::_this;

Router::Router(const ConfigSPtr& config): _thread(0) {
}

Router::~Router() {
  stop();
}

bool Router::start() {
  _threadState = Start;
  int ret = pthread_create(&_thread, NULL, Router::threadFunc, this);
  if (0 != ret) {
    log.error("Router Start Fail: %d\n", ret);
  } else {
    while (_threadState != Run && _threadState != Terminate);
  }
  return !ret;
}

bool Router::stop() {
  if (0 == _thread) {
    return false;
  }
  pthread_kill(_thread, SIGTERM);
  // TODO: decide return;
  int* ret;
  pthread_join(_thread, (void**)&ret);
  log.debug("Thread Stop!! %d\n", _thread);
  bool succ = (0 == *ret);
  delete ret;
  return succ;
}

void* Router::threadFunc(void* obj) {
  Router* router = (Router*)obj;
  router->_threadState = Run;
  router->_this = router;
  log.debug("Thread Start!! %d\n", router->_thread);
  signal(SIGTERM, Router::handleTERM);
  return router->mainLoop();
}

void* Router::mainLoop() {
  while (_threadState == Run);
  int* ret = new int();
  *ret = 0;
  return ret;
}

void Router::handleTERM(int sig) {
  signal(SIGTERM, NULL);
  log.debug("Thread TERM\n");
  Router::_this->_threadState = Terminate;
}

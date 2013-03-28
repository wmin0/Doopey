#include "network/Router.h"

#include "config/Config.h"
#include "common/Doopey.h"

#include <csignal>
#include <pthread.h>

using namespace Doopey;

Router* Router::_this = NULL;

Router::Router(const ConfigSPtr& config): _thread(0) {
}

Router::~Router() {
  stop();
}

bool Router::start() {
  if (0 != _thread || NULL != Router::_this) {
    return false;
  }
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
  pthread_kill(_thread, SIGRSTOP);
  // TODO: decide return;
  int* ret;
  pthread_join(_thread, (void**)&ret);
  log.debug("Thread Stop!! %ld\n", _thread);

  // reset
  signal(SIGRSTOP, NULL);
  signal(SIGRREQ, NULL);
  _thread = 0;
  Router::_this = NULL;
  bool succ = (0 == *ret);
  delete ret;
  return succ;
}

void* Router::threadFunc(void* obj) {
  Router* router = (Router*)obj;
  log.debug("Thread Start!! %ld\n", router->_thread);
  signal(SIGRSTOP, Router::handleRSTOP);
  signal(SIGRREQ, Router::handleRREQ);
  router->_this = router;
  router->_threadState = Run;
  return router->mainLoop();
}

void* Router::mainLoop() {
  while (_threadState == Run);
  int* ret = new int(0);
  return ret;
}

void Router::handleRSTOP(int sig) {
  log.debug("Thread RSTOP\n");
  Router::_this->_threadState = Terminate;
}

void Router::handleRREQ(int sig) {
}

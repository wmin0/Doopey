#include "network/Dispatcher.h"

#include "common/Doopey.h"
#include "machine/Server.h"

#include <csignal>
#include <pthread.h>

using namespace Doopey;

Dispatcher* Dispatcher::_this = NULL;

Dispatcher::Dispatcher(const Server* server, const ConfigSPtr& config):
  _server(server), _thread(0), _threadState(TS_None) {
}

Dispatcher::~Dispatcher() {
  stop();
}

bool Dispatcher::start() {
  if (0 != _thread || NULL != Dispatcher::_this) {
    return false;
  }
  _threadState = TS_Start;
  int ret = pthread_create(&_thread, NULL, Dispatcher::threadFunc, this);
  if (0 != ret) {
    log.error("Dispatcher Start Fail: %d\n", ret);
  } else {
    while (_threadState != TS_Run && _threadState != TS_Terminate);
  }
  return !ret;
}

bool Dispatcher::stop() {
  if (0 == _thread) {
    return false;
  }
  pthread_kill(_thread, SIGDSTOP);
  // TODO: decide return;
  int* ret;
  pthread_join(_thread, (void**)&ret);
  log.debug("Thread Stop!! %ld\n", _thread);

  // reset
  signal(SIGDSTOP, NULL);
  _thread = 0;
  Dispatcher::_this = NULL;
  bool succ = (0 == *ret);
  delete ret;
  return succ;
}

void* Dispatcher::threadFunc(void* obj) {
  Dispatcher* dispatcher = (Dispatcher*)obj;
  log.debug("Thread Start!! %ld\n", dispatcher->_thread);
  signal(SIGDSTOP, Dispatcher::handleDSTOP);
  Dispatcher::_this = dispatcher;
  dispatcher->_threadState = TS_Run;
  return dispatcher->mainLoop();
}

void* Dispatcher::mainLoop() {
  while (_threadState == TS_Run);
  int* ret = new int(0);
  return ret;
}

void Dispatcher::handleDSTOP(int sig) {
  log.debug("Thread DSTOP\n");
  Dispatcher::_this->_threadState = TS_Terminate;
}

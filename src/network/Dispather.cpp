#include "network/Dispatcher.h"

#include "common/Doopey.h"
#include "common/Thread.h"
#include "machine/Server.h"

#include <pthread.h>

using namespace Doopey;

Dispatcher::Dispatcher(const Server* server, const ConfigSPtr& config):
  _server(server), _run(false) {
  _thread.reset(new Thread(threadFunc, threadStop));
}

Dispatcher::~Dispatcher() {
  stop();
}

bool Dispatcher::start() {
  log.debug("Dispatcher Thread start!!\n");
  return _thread->start(this);
}

bool Dispatcher::stop() {
  log.debug("Dispatcher Thread stop!!\n");
  return _thread->stop(this);
}

void Dispatcher::threadFunc(void* obj) {
  log.debug("Dispatcher Func!!\n");
  Dispatcher* dispatcher = (Dispatcher*)obj;
  dispatcher->_run = true;
  dispatcher->mainLoop();
}

void Dispatcher::threadStop(void* obj) {
  Dispatcher* dispatcher = (Dispatcher*)obj;
  dispatcher->_run = false;
}

void Dispatcher::mainLoop() {
  while (_run);
}


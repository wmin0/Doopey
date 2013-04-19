#include "network/Router.h"

#include "common/Config.h"
#include "common/Doopey.h"
#include "common/Thread.h"
#include "network/Message.h"
#include "network/Socket.h"

#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace Doopey;
using namespace std;

Router* Router::_this = NULL;

const uint32_t Router::heartBeatInterval = 30;

Router::Router(const Server* server, const ConfigSPtr& config):
  _server(server), _run(false) {
  _thread.reset(new Thread(threadFunc, threadStop));
  string list = config->getValue("connectList");
  if ("" != list) {
    initTopology(DoopeyRoot + list);
  }
}

Router::~Router() {
  stop();
}

bool Router::initTopology(const string& list) {
  fstream file(list, fstream::in);
  if (!file.good()) {
    log->error("no found machine list %s\n", list.data());
    return false;
  }
  string buf;
  while (getline(file, buf)) {
    initConnectNeignbor(buf);
  }
  return true;
}

bool Router::initConnectNeignbor(const string& ip) {
  log->info("connect to %s\n", ip.data());
  Socket sock(ST_TCP);
  if (!sock.connect(ip, DoopeyPort)) {
    log->warning("connect to %s fail\n", ip.data());
    return false;
  }
  MessageSPtr msg(new Message(MT_Router, MC_NeighborInit));
  sock.send(msg);
  //sock.receive()
  return true;
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
  while (_run) {
    sleep(Router::heartBeatInterval);
  }
}

void Router::threadStop(void* obj) {
  Router* router = (Router*)obj;
  router->_run = false;
}


#include "network/Router.h"

#include "common/Config.h"
#include "common/Doopey.h"
#include "common/Thread.h"
#include "machine/Server.h"
#include "network/Message.h"
#include "network/Socket.h"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>

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
  // NOTE: at neignbor side, add this machine by update routing,
  //       because this machine can send machineID at that time.
  MessageSPtr msg(new Message(MT_Router, MC_NeighborInit));
  if (!sock.send(msg)) {
    log->warning("send register msg to %s fail\n", ip.data());
    return false;
  }
  MessageSPtr ack = sock.receive();
  if (NULL == ack) {
    log->warning("recv register msg from %s fail\n", ip.data());
    return false;
  }
  if (MT_Router != ack->getType() && MC_RouterACK != ack->getCmd()) {
    log->warning("register ack error from %s\n");
    return false;
  }
  if (sizeof(MachineID) != ack->getData().size()) {
    log->warning("wrong ack msg from %s\n");
    return false;
  }
  MachineID id = *(MachineID*)(ack->getData().data());
  return addRoutingPath(id, ip);
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


// routing operation functions
bool Router::addRoutingPath(const MachineID id, const string& ip) {
  // TODO: warning when rewrite
  _routingTable[id] = ip;
  return true;
}

// handle request functions
void Router::request(const MessageSPtr& msg, const SocketSPtr& sock) {
  if (MT_Router != msg->getType()) {
    return;
  }
  switch (msg->getCmd()) {
    case MC_NeighborInit:
      handleNeighborInit(sock);
      break;
    default:
      return;
  }
}

bool Router::handleNeighborInit(const SocketSPtr& sock) {
  MessageSPtr ack(new Message(MT_Router, MC_RouterACK));
  MachineID id = _server->getMachineID();
  ack->addData((unsigned char*)&id, 0, sizeof(MachineID));
  if (!sock->send(ack)) {
    log->warning("send register ack fail\n");
    return false;
  }
  return true;
}

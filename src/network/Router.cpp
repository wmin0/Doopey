#include "network/Router.h"

#include "common/Config.h"
#include "common/Doopey.h"
#include "common/Thread.h"
#include "machine/Server.h"
#include "common/Message.h"
#include "common/Socket.h"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <memory.h>

using namespace Doopey;
using namespace std;

Router* Router::_this = NULL;

const uint32_t Router::heartBeatInterval = 30;

Router::Router(Server* server, const ConfigSPtr& config):
  _server(server), _run(false) {
  _thread.reset(new Thread(threadFunc, threadStop));
  string list = config->getValue("connectList");
  if ("" != list) {
    initTopology(DoopeyRoot + list);
  }
  initMachineID();
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
    initConnectNeighbor(buf);
  }
  return true;
}

bool Router::initConnectNeighbor(const string& ip) {
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
  return addRoutingPath(id, ip, 1);
}

void Router::initMachineID() {
  if (0 == _neighbors.size()) {
    // NOTE: first node
    _server->setMachineIDMax(1);
    _server->setMachineID(1);
    log->info("set MachineID(1)\n");
    log->info("set MachineIDMax(1)\n");
    return;
  }
  RoutingMap::iterator it = _routingTable.begin();
  MachineID max = 0;
  while (_routingTable.end() != it) {
    do {
      Socket sock(ST_TCP);
      if (!sock.connect(it->second.ip, DoopeyPort)) {
        log->warning("connect to %s fail\n", it->second.ip.data());
        break;
      }
      MessageSPtr msg(new Message(MT_Router, MC_MachineIDMax));
      if (!sock.send(msg)) {
        log->warning("send MaxMachineID msg to %s fail\n", it->second.ip.data());
        break;
      }
      MessageSPtr ack = sock.receive();
      if (NULL == ack) {
        log->warning("recv msg from %s fail\n", it->second.ip.data());
        break;
      }
      if (MT_Router != ack->getType() && MC_RouterACK != ack->getCmd()) {
        log->warning("MaxMachineID ack error from %s\n");
        break;
      }
      if (sizeof(MachineID) != ack->getData().size()) {
        log->warning("wrong ack msg from %s\n");
        break;
      }
      MachineID id = *(MachineID*)(ack->getData().data());
      if (id > max) {
        max = id;
      }
    } while(0);
    ++it;
  }
  ++max;
  _server->setMachineIDMax(max);
  _server->setMachineID(max);
  log->info("set MachineID(%d)\n", max);
  log->info("set MachineIDMax(%d)\n", max);
  MessageSPtr update(new Message(MT_Router, MC_UpdateMachineIDMax));
  size_t off = 0;
  update->addData((unsigned char*)&max, off, sizeof(MachineID));
  off += sizeof(MachineID);
  string local = _server->getLocalIP();
  size_t len = local.size();
  update->addData((unsigned char*)&len, off, sizeof(size_t));
  off += sizeof(size_t);
  update->addData((unsigned char*)local.data(), off, len);
  broadcast(update);
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

void Router::broadcast(const MessageSPtr& msg) const {
  RoutingMap::const_iterator it = _routingTable.begin();
  while (_routingTable.end() != it) {
    sendTo(it->first, msg);
    ++it;
  }
}

bool Router::sendTo(MachineID id, const MessageSPtr& msg) const {
  RoutingMap::const_iterator it = _routingTable.find(id);
  if (_routingTable.end() == it) {
    return false;
  }
  msg->setSrc(_server->getMachineID());
  msg->setDest(id);
  Socket sock(ST_TCP);
  if (!sock.connect(it->second.ip, DoopeyPort)) {
    // TODO: clear record
    return false;
  }
  if (!sock.send(msg)) {
    return false;
  }
  return true;
}

// routing operation functions
bool Router::addRoutingPath(
  const MachineID id, const string& ip, const uint16_t& d) {
  // TODO: warning when rewrite
  _routingTable[id] = RoutingEntry(ip, d);
  if (1 == d) {
    _neighbors.insert(id);
  }
  log->info("update routing <%d, %s, %d>\n", id, ip.data(), d);
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
    case MC_MachineIDMax:
      handleMachineIDMax(sock);
      break;
    case MC_UpdateMachineIDMax:
      handleUpdateMachineIDMax(msg);
      break;
    default:
      break;
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

bool Router::handleMachineIDMax(const SocketSPtr& sock) {
  MessageSPtr ack(new Message(MT_Router, MC_RouterACK));
  MachineID id = _server->getMachineIDMax();
  ack->addData((unsigned char*)&id, 0, sizeof(MachineID));
  if (!sock->send(ack)) {
    log->warning("send register ack fail\n");
    return false;
  }
  return true;
}

bool Router::handleUpdateMachineIDMax(const MessageSPtr& msg) {
  size_t off = 0;
  MachineID max = *(MachineID*)(msg->getData().data());
  off += sizeof(MachineID);
  size_t len = *(size_t*)(msg->getData().data() + off);
  off += sizeof(size_t);
  string ip;
  ip.resize(len);
  char* buf = new char[len];
  memcpy(buf, msg->getData().data() + off, len);
  ip = buf;
  delete[] buf;
  _server->setMachineIDMax(max);
  log->info("set MachineIDMax(%d)\n", max);
  return addRoutingPath(msg->getSrc(), ip, 1);
}

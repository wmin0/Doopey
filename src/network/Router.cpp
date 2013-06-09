#include "network/Router.h"

#include "block/Block.h"
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
#include <cstdlib>

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
  initTable();
  updateMachineIDMax();
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
  if (0 != _server->getMachineID()) {
    // have snapshot
    MachineID oldID = _server->getMachineID();
    log->info("try to reg old machineID %d\n", oldID);
    if (0 == _neighbors.size()) {
      _server->setMachineIDMax(oldID);
      log->info("set MachineID(%d)\n", oldID);
      log->info("set MachineIDMax(%d)\n", oldID);
      return;
    }
    do {
      RoutingMap::iterator it = _routingTable.begin();
      Socket sock(ST_TCP);
      if (!sock.connect(it->second.ip, DoopeyPort)) {
        log->warning("connect to %s fail\n", it->second.ip.data());
        break;
      }
      MessageSPtr msg(new Message(MT_Router, MC_CheckRepeatMachineID));
      msg->addData((unsigned char*)&oldID, 0, sizeof(MachineID));
      string localIP = _server->getLocalIP();
      uint64_t len = localIP.size();
      msg->addData((unsigned char*)&len, sizeof(MachineID), sizeof(uint64_t));
      msg->addData((unsigned char*)localIP.data(),
                   sizeof(MachineID) + sizeof(uint64_t),
                   len);
      if (!sock.send(msg)) {
        log->warning("send CheckRepeatMachineID msg to %s fail\n", it->second.ip.data());
        break;
      }
      MessageSPtr ack = sock.receive();
      if (NULL == ack) {
        log->warning("recv msg from %s fail\n", it->second.ip.data());
        break;
      }
      if (MT_Router != ack->getType() && MC_RouterACK != ack->getCmd()) {
        log->warning("CheckRepeatMachineID ack error from %s\n");
        break;
      }
      bool ans;
      MachineID max;
      memcpy(&ans, ack->getData().data(), sizeof(bool));
      memcpy(&max, ack->getData().data() + sizeof(bool), sizeof(MachineID));
      if (max < oldID) {
        max = oldID;
      }
      if (ans) {
        _server->setMachineIDMax(max);
        log->info("set MachineID(%d)\n", oldID);
        log->info("set MachineIDMax(%d)\n", max);
        return;
      }
    } while(0);
  }
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
  string localIP = _server->getLocalIP();
  while (_routingTable.end() != it) {
    do {
      if (localIP == it->second.ip) {
        continue;
      }
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
}

void Router::initTable() {
  if (0 == _neighbors.size()) {
    return;
  }
  log->info("fetching routing table\n");
  RoutingMap::iterator it = _routingTable.begin();
  bool done = false;
  while (_routingTable.end() != it) {
    log->info("try to connect to %s\n", it->second.ip.data());
    do {
      Socket sock(ST_TCP);
      if (!sock.connect(it->second.ip, DoopeyPort)) {
        log->warning("connect to %s fail\n", it->second.ip.data());
        break;
      }
      MessageSPtr msg(new Message(MT_Router, MC_RequestRoutingTable));
      if (!sock.send(msg)) {
        log->warning("send RequestTable msg to %s fail\n", it->second.ip.data());
        break;
      }
      MessageSPtr ack = sock.receive();
      if (NULL == ack) {
        log->warning("recv msg from %s fail\n", it->second.ip.data());
        break;
      }
      if (MT_Router != ack->getType() && MC_RouterACK != ack->getCmd()) {
        log->warning("RequestTable ack error from %s\n");
        break;
      }
      uint64_t total = ack->getData().size();
      uint64_t off = 0;
      while (total != off) {
        MachineID id;
        uint64_t len;
        string ip;
        uint16_t d;
        memcpy(&id, ack->getData().data() + off, sizeof(MachineID));
        off += sizeof(MachineID);
        memcpy(&len, ack->getData().data() + off, sizeof(uint64_t));
        off += sizeof(uint64_t);
        ip.resize(len);
        memcpy(&(ip[0]), ack->getData().data() + off, len);
        off += len;
        memcpy(&d, ack->getData().data() + off, sizeof(uint16_t));
        off += sizeof(uint16_t);
        if (_server->getLocalIP() == ip) {
          continue;
        }
        // TODO: len
        addRoutingPath(id, ip, d + 1);
        done = true;
      }
    } while(0);
    if (done) {
      break;
    }
    ++it;
  }
}

void Router::updateMachineIDMax() {
  MessageSPtr update(new Message(MT_Router, MC_UpdateMachineIDMax));
  uint64_t off = 0;
  MachineID max = _server->getMachineIDMax();
  update->addData((unsigned char*)&max, off, sizeof(MachineID));
  off += sizeof(MachineID);
  string local = _server->getLocalIP();
  uint64_t len = local.size();
  update->addData((unsigned char*)&len, off, sizeof(uint64_t));
  off += sizeof(uint64_t);
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

vector<MachineID> Router::pickMachineBesideList(
  const vector<MachineID>& list, size_t num) const {
  vector<MachineID> ret;
  RoutingMap::const_iterator it = _routingTable.begin();
  while (_routingTable.end() != it) {
    int j = rand() % _routingTable.size();
    it = _routingTable.begin();
    for (int i = 0; i < j; ++i) {
      ++it;
    }
    if (num == 0 || num > Block::blockReplica) {
      break;
    }
    bool in = false;
    for (size_t i = 0; i < list.size(); ++i) {
      if (list[i] == it->first) {
        in = true;
      }
    }
    for (size_t i = 0; i < ret.size(); ++i) {
      if (ret[i] == it->first) {
        in = true;
      }
    }
    if (in) {
      continue;
    }
    ret.push_back(it->first);
    --num;
  }
  return ret;
}

void Router::broadcast(const MessageSPtr& msg) {
  RoutingMap::const_iterator it = _routingTable.begin();
  while (_routingTable.end() != it) {
    sendTo(it->first, msg);
    ++it;
  }
}

SocketSPtr Router::sendTo(MachineID id, const MessageSPtr& msg) {
  RoutingMap::iterator it = _routingTable.find(id);
  if (_routingTable.end() == it) {
    log->warning("can't route to %d\n", id);
    return NULL;
  }
  msg->setSrc(_server->getMachineID());
  msg->setDest(id);
  SocketSPtr sock(new Socket(ST_TCP));
  if (!sock->connect(it->second.ip, DoopeyPort)) {
    _routingTable.erase(it);
    return NULL;
  }
  if (!sock->send(msg)) {
    return NULL;
  }
  return sock;
}

// routing operation functions
bool Router::addRoutingPath(
  const MachineID id, const string& ip, const uint16_t& d) {
  // TODO: warning when rewrite
  // TODO: choose short if rewrite
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
    case MC_RequestRoutingTable:
      handleRequestRoutingTable(sock);
      break;
    case MC_CheckRepeatMachineID:
      handleCheckRepeatMachineID(sock, msg);
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
  uint64_t off = 0;
  MachineID max = *(MachineID*)(msg->getData().data());
  off += sizeof(MachineID);
  uint64_t len = *(uint64_t*)(msg->getData().data() + off);
  off += sizeof(uint64_t);
  string ip;
  ip.resize(len);
  memcpy(&(ip[0]), msg->getData().data() + off, len);
  MachineID localMax = _server->getMachineIDMax();
  if (localMax >= max) {
    log->warning("ignore max: %d, local max: %d\n", max, localMax);
  } else {
    _server->setMachineIDMax(max);
    log->info("set MachineIDMax(%d)\n", max);
  }
  return addRoutingPath(msg->getSrc(), ip, 1);
}

bool Router::handleRequestRoutingTable(const SocketSPtr& sock) {
  MessageSPtr ack(new Message(MT_Router, MC_RouterACK));
  RoutingMap::const_iterator it = _routingTable.begin();
  uint64_t off = 0;
  while (_routingTable.end() != it) {
    ack->addData((unsigned char*)&(it->first), off, sizeof(MachineID));
    off += sizeof(MachineID);
    uint64_t len = it->second.ip.size();
    ack->addData((unsigned char*)&(len), off, sizeof(uint64_t));
    off += sizeof(uint64_t);
    ack->addData((unsigned char*)it->second.ip.data(), off, len);
    off += len;
    ack->addData((unsigned char*)&(it->second.d), off, sizeof(uint16_t));
    off += sizeof(uint16_t);
    ++it;
  }
  if (!sock->send(ack)) {
    log->warning("send routing table ack fail\n");
    return false;
  }
  return true;
}

bool Router::handleCheckRepeatMachineID(
  const SocketSPtr& sock, const MessageSPtr& msg) {
  MachineID req;
  memcpy(&req, msg->getData().data(), sizeof(MachineID));
  string IP;
  uint64_t len = 0;
  memcpy(&len, msg->getData().data() + sizeof(MachineID), sizeof(uint64_t));
  IP.resize(len);
  memcpy(&(IP[0]),
         msg->getData().data() + sizeof(MachineID) + sizeof(uint64_t),
         len);
  // true means available
  bool ans = true;
  RoutingMap::iterator it = _routingTable.find(req);
  if (_server->getMachineID() == req ||
      (_routingTable.end() != it && IP != it->second.ip)) {
    ans = false;
  }
  MessageSPtr ack(new Message(MT_Router, MC_RouterACK));
  ack->addData((unsigned char*)&ans, 0, sizeof(bool));
  MachineID max = _server->getMachineIDMax();
  ack->addData((unsigned char*)&max, sizeof(bool), sizeof(MachineID));
  if (!sock->send(ack)) {
    log->warning("send CheckRepeatMachineID ack fail\n");
    return false;
  }
  return true;
}

string Router::askMachineIP(const MachineID& id) const {
  if (_server->getMachineID() == id) {
    return _server->getLocalIP();
  }
  RoutingMap::const_iterator it = _routingTable.find(id);
  if (_routingTable.end() == it) {
    return "";
  }
  return it->second.ip;
}

MachineID Router::pickNeighborMachine() const {
  if (0 == _neighbors.size()) {
    return 0;
  }
  return *(_neighbors.begin());
}

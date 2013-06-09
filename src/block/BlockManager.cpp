#include "block/BlockManager.h"

#include "block/Block.h"
#include "block/BlockLoader.h"
#include "block/BlockResolver.h"
#include "block/BlockSaver.h"
#include "block/BlockLocationAttr.h"
#include "block/BlockUpdater.h"
#include "block/DataBlock.h"
#include "block/MetaBlock.h"
#include "common/Config.h"
#include "common/Message.h"
#include "common/Thread.h"
#include "common/Socket.h"
#include "network/Router.h"
#include "machine/Server.h"

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <memory.h>
#include <pthread.h>

using namespace Doopey;
using namespace std;

const MachineID& BlockManager::getMachineID() const {
  return _server->getMachineID();
}

const RouterSPtr& BlockManager::getRouter() const {
  return _server->getRouter();
}

BlockManager::BlockManager(Server* server, const ConfigSPtr& config):
  _server(server), _localDir(".") {
  if (NULL != config) {
    string tmp = config->getValue("BlockDir");
    if ("" != tmp) {
      _localDir = DoopeyRoot + tmp;
    } else {
      log->warning("Use Default BlockDir .\n");
    }
  } else {
    log->warning("Use Default BlockResolver .\n");
  }
  _resolver.reset(new BlockResolver(this, config));
  _loader.reset(new BlockLoader(this, config));
  _saver.reset(new BlockSaver(this, config));
  _updater.reset(new BlockUpdater(this, config));
  pthread_mutex_init(&BlockResolver::_sig_lock, NULL);
}

BlockManager::~BlockManager() {
  pthread_mutex_destroy(&BlockResolver::_sig_lock);
}

MetaBlockSPtr BlockManager::newMeta() {
  return _loader->newMeta();
}

DataBlockSPtr BlockManager::newData() {
  return _loader->newData();
}

MetaBlockSPtr BlockManager::getMeta(BlockID id) {
  BlockLocationAttrSPtr attr = _resolver->askBlock(id);
  if (NULL == attr) {
    log->warning("no found meta block: %lld\n", id);
    return NULL;
  }
  return _loader->loadMeta(attr);
}

DataBlockSPtr BlockManager::getData(BlockID id) {
  BlockLocationAttrSPtr attr = _resolver->askBlock(id);
  if (NULL == attr) {
    log->warning("no found data block: %lld\n", id);
    return NULL;
  }

  return _loader->loadData(attr);
}

BlockID BlockManager::saveBlock(const BlockSPtr& block) {
  BlockID oldID = block->getID();
  BlockID newID = _saver->saveBlock(block);
  if (0 != oldID && newID != oldID) {
    // TODO: updater? invalid?
  }
  return newID;
}

void BlockManager::deleteBlock(BlockID id) {
  // TODO:
  BlockLocationAttrSPtr attr = _resolver->askBlockDetail(id);
  if (NULL == attr) {
    return;
  }
  if (0 == attr->machine.size()) {
    return;
  }
  MessageSPtr msg(new Message(MT_Block, MC_DoDelete));
  msg->addData((unsigned char*)&id, sizeof(BlockID));
  if (getMachineID() == attr->machine[0]) {
    handleDoDelete(msg);
  } else {
    getRouter()->sendTo(attr->machine[0], msg);
  }
}

string BlockManager::convertBlockIDToPath(const BlockID& id) const {
  stringstream ss("");
  ss << _localDir << "/"
     << internal << setfill('0') << uppercase
     << hex << setw(16) << id;
  return ss.str();
}

bool BlockManager::isHealth() const {
  return _resolver->isHealth();
}

void BlockManager::request(const MessageSPtr& msg, const SocketSPtr& sock) {
  switch (msg->getCmd()) {
    case MC_RequestBlockLocation:
      log->debug("RequestBlockLocation\n");
      _resolver->handleRequestBlockLocation(msg);
      break;
    case MC_RequestBlockLocationACK:
      log->debug("RequestBlockLocationACK\n");
      _resolver->handleRequestBlockLocationACK(msg);
      break;
    case MC_RequestBlockData:
      log->debug("RequestBlockData\n");
      _loader->handleRequestBlockData(sock, msg);
      break;
    case MC_CheckBlockAlive:
      log->debug("RequestBlockAlive\n");
      _resolver->handleCheckBlockAlive(sock, msg);
      break;
    case MC_DoReplica:
      log->debug("DoReplica\n");
      handleDoReplica(msg);
      break;
    case MC_UpdateReplica:
      log->debug("UpdateReplica\n");
      _resolver->handleUpdateReplica(msg);
      break;
    case MC_CopyBlockFromRemote:
      log->debug("CopyBlockFromRemote\n");
      handleCopyBlockFromRemote(sock, msg);
      break;
    case MC_DoDelete:
      log->debug("DoDelete\n");
      handleDoDelete(msg);
      break;
    case MC_DeleteBlock:
      log->debug("DeleteBlock\n");
      handleDeleteBlock(msg);
      break;
    default:
      log->warning("unknown block request\n");
      break;
  }
}

bool BlockManager::handleDoReplica(const MessageSPtr& msg) {
  // TODO:
  BlockID id;
  memcpy(&id, msg->getData().data(), sizeof(BlockID));
  BlockLocationAttrSPtr attr = _resolver->askBlockDetail(id);
  if (NULL == attr) {
    log->warning("receive wrong DoReplica Request: %lld\n", id);
    return false;
  }
  if (BS_Available != attr->state) {
    log->warning("invalid request replica: %lld\n", id);
    return false;
  }
  // change to remote list
  size_t off = sizeof(BlockID);
  MachineID mid;
  attr->machine.clear();
  while (off < msg->getData().size()) {
    memcpy(&mid, msg->getData().data() + off, sizeof(MachineID));
    off += sizeof(MachineID);
    attr->addMachine(mid);
  }

  vector<MachineID> candidate =
    getRouter()->pickMachineBesideList(
      attr->machine, Block::blockReplica - attr->machine.size());
  MessageSPtr cmd(new Message(MT_Block, MC_CopyBlockFromRemote));
  cmd->addData((unsigned char*)&id, 0, sizeof(BlockID));
  for (size_t i = 0; i < candidate.size(); ++i) {
    SocketSPtr sock = getRouter()->sendTo(candidate[i], cmd);
    if (NULL == sock) {
      continue;
    }
    MessageSPtr ack = sock->receive();
    if (NULL != ack &&
        MT_Block == ack->getType() &&
        MC_BlockACK == ack->getCmd()) {
      attr->addMachine(candidate[i]);
    }
  }
  // TODO: update
  _updater->updateRemoteBlockInfo(attr);
  return true;
}

bool BlockManager::handleCopyBlockFromRemote(
  // TODO: check local loading
  const SocketSPtr& sock, const MessageSPtr& msg) {
  BlockID id;
  MachineID mid = msg->getSrc();
  memcpy(&id, msg->getData().data(), sizeof(BlockID));
  BlockLocationAttrSPtr attr(new BlockLocationAttr(id, 0, BS_Available));
  attr->addMachine(mid);
  DataBlockSPtr block = _loader->loadData(attr);
  if (NULL != block) {
    if (0 != _saver->saveBlock((BlockSPtr)block)) {
      MessageSPtr ack(new Message(MT_Block, MC_BlockACK));
      sock->send(ack);
    }
  }
  return true;
}

bool BlockManager::handleDoDelete(const MessageSPtr& msg) {
  BlockID id;
  memcpy(&id, msg->getData().data(), sizeof(Block));

  BlockLocationAttrSPtr attr = _resolver->askBlockDetail(id);
  if (NULL == attr) {
    log->warning("invalid delete %lld\n", id);
    return false;
  }
  // keep state
  attr->state = BS_Deleting;
  MessageSPtr cmd(new Message(MT_Block, MC_DeleteBlock));
  cmd->addData(msg->getData().data(), sizeof(Block));
/*
  for (size_t i = 1; i < attr->machine.size(); ++i) {
    getRouter()->sendTo(attr->machine[i], cmd);
  }
*/
  getRouter()->broadcast(cmd);
  return handleDeleteBlock(cmd);
}

bool BlockManager::handleDeleteBlock(const MessageSPtr& msg) {
  BlockID id;
  memcpy(&id, msg->getData().data(), sizeof(Block));
  _resolver->removeLocalID(id);
  return true;
}

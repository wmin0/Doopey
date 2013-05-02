#include "block/BlockLoader.h"

#include "block/Block.h"
#include "block/BlockLocationAttr.h"
#include "block/DataBlock.h"
#include "block/MetaBlock.h"
#include "block/BlockManager.h"
#include "network/Router.h"
#include "common/Socket.h"
#include "common/Message.h"

#include <fstream>
#include <iostream>
#include <memory.h>

using namespace Doopey;
using namespace std;

BlockLoader::BlockLoader(const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager) {
}

BlockLoader::~BlockLoader() {
}

DataBlockSPtr BlockLoader::loadData(const BlockLocationAttrSPtr& attr) const {
  unsigned char* data = getData(attr);
  if (NULL != data) {
    return DataBlockSPtr(new DataBlock(data, attr->block));
  }
  return DataBlockSPtr(NULL);
}

MetaBlockSPtr BlockLoader::loadMeta(const BlockLocationAttrSPtr& attr) const {
  unsigned char* data = getData(attr);
  if (NULL != data) {
    return MetaBlockSPtr(new MetaBlock(data, attr->block));
  }
  return MetaBlockSPtr(NULL);
}

unsigned char* BlockLoader::getData(const BlockLocationAttrSPtr& attr) const {
  if (BS_Available != attr->state) {
    return NULL;
  }
  MachineID local = _manager->getMachineID();
  bool atLocal = false;
  if (0 == attr->machine.size()) {
    return NULL;
  }
  for (size_t i = 0; i < attr->machine.size(); ++i) {
    if (local == attr->machine[i]) {
      atLocal = true;
    }
  }
  unsigned char* mem = allocateMem();
  if (!mem) {
    return NULL;
  }
  bool succ = false;
  do {
    if (atLocal) {
      string path = _manager->convertBlockIDToPath(attr->block);
      fstream file(path, fstream::in | fstream::binary);
      if (!file.good()) {
        log->warning("load %s fail\n", path.data());
        break;
      }
      log->debug("load %s\n", path.data());
      file >> mem;
      file.close();
      succ = true;
    } else {
      RouterSPtr router = _manager->getRouter();
      MessageSPtr msg(new Message(MT_Block, MC_RequestBlockData));
      msg->addData((unsigned char*)&(attr->block), 0, sizeof(BlockID));
      // TODO: load balance
      log->debug("try to load remote %d->%d\n", attr->machine[0], attr->block);
      SocketSPtr sock = router->sendTo(attr->machine[0], msg);
      if (NULL == sock) {
        break;
      }
      MessageSPtr ack = sock->receive();
      if (NULL == ack) {
        break;
      }
      if (MT_Block != ack->getType() ||
          MC_BlockACK != ack->getCmd() ||
          Block::blockSize != ack->getData().size()) {
        break;
      }
      log->debug("load success\n");
      memcpy(mem, ack->getData().data(), Block::blockSize);
      succ = true;
    }
  } while(0);
  if (!succ) {
    delete[] mem;
    mem = NULL;
  }
  return mem;
}

DataBlockSPtr BlockLoader::newData() const {
  // NOTE: allocate id by Saver, because Saver know about local information
  // NOTE: don't use vector here for controlling memory.
  unsigned char* mem = allocateMem();
  if (NULL == mem) {
    return DataBlockSPtr(NULL);
  }
  return DataBlockSPtr(new DataBlock(mem, 0));
}
MetaBlockSPtr BlockLoader::newMeta() const {
  unsigned char* mem = allocateMem();
  if (NULL == mem) {
    return MetaBlockSPtr(NULL);
  }
  return MetaBlockSPtr(new MetaBlock(mem, 0));
}

unsigned char* BlockLoader::allocateMem() const {
  unsigned char* mem = NULL;
  try {
    mem = new unsigned char[Block::blockSize];
  } catch (...) {
    mem = NULL;
  }
  return mem;
}

bool BlockLoader::handleRequestBlockData(const SocketSPtr& sock, const MessageSPtr& msg) {
  BlockID id;
  memcpy(&id, msg->getData().data(), sizeof(BlockID));
  unsigned char* mem = allocateMem();
  if (!mem) {
    return false;
  }
  string path = _manager->convertBlockIDToPath(id);
  fstream file(path, fstream::in | fstream::binary);
  if (!file.good()) {
    return false;
  }
  file >> mem;
  file.close();
  MessageSPtr ack(new Message(MT_Block, MC_BlockACK));
  ack->addData(mem, 0, Block::blockSize);
  sock->send(ack);
  delete[] mem;
  return true;
}

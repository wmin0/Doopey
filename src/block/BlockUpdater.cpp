#include "block/BlockUpdater.h"

#include "common/Message.h"
#include "block/BlockLocationAttr.h"
#include "block/BlockManager.h"
#include "network/Router.h"

using namespace Doopey;

BlockUpdater::BlockUpdater(const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager) {
}

BlockUpdater::~BlockUpdater() {
}

void BlockUpdater::updateRemoteBlockInfo(const BlockLocationAttrSPtr& attr) {
  MessageSPtr msg(new Message(MT_Block, MC_UpdateReplica));
  size_t off = 0;
  msg->addData((unsigned char*)&(attr->block), off, sizeof(BlockID));
  off += sizeof(BlockID);
  for (size_t i = 0; i < attr->machine.size(); ++i) {
    msg->addData((unsigned char*)&(attr->machine[i]), off, sizeof(MachineID));
  off += sizeof(MachineID);
  }
  const RouterSPtr router = _manager->getRouter();
/*
  for (size_t i = 0; i < attr->machine.size(); ++i) {
    router->sendTo(attr->machine[i], msg);
  }
*/
  router->broadcast(msg);
}

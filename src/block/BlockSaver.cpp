#include "block/BlockSaver.h"

using namespace Doopey;

BlockSaver::BlockSaver(const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager), _localDir(".") {
  // TODO: check local ids
}

BlockSaver::~BlockSaver() {
}

BlockID BlockSaver::saveBlock(const BlockSPtr& block) {
  // TODO: save local and update Resolver
  return true;
}

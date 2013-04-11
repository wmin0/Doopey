#include "block/BlockSaver.h"

using namespace Doopey;

BlockSaver::BlockSaver(const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager), _localDir(".") {
}

BlockSaver::~BlockSaver() {
}

BlockID BlockSaver::saveBlock(const BlockSPtr& block) {
  // TODO: save local and ask resolver for id
  return true;
}

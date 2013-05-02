#include "block/BlockSaver.h"

#include "block/Block.h"
#include "block/BlockManager.h"
#include "block/BlockResolver.h"

#include "common/Doopey.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace Doopey;
using namespace std;

BlockSaver::BlockSaver(const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager) {
}

BlockSaver::~BlockSaver() {
}

BlockID BlockSaver::saveBlock(const BlockSPtr& block) {
  const BlockResolverSPtr resolver = _manager->getBlockResolver();
  BlockID newLocalBID = resolver->newLocalID();
  // get a new ID for file name

  string path = _manager->convertBlockIDToPath(newLocalBID);
  fstream file(path, fstream::out | fstream::binary);
  //create new file, filename is "locatDir+newLocalBID"

  if (!file.good()){
    log->error("create file failed.");
    return 0;
  } else {
    file << block->_data;
    file.close();
    resolver->addLocalID(newLocalBID);
    return newLocalBID;
  }
}

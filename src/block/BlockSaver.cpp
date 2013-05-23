#include "block/BlockSaver.h"

#include "block/Block.h"
#include "block/BlockManager.h"
#include "block/BlockResolver.h"

#include "common/Doopey.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace Doopey;
using namespace std;

BlockSaver::BlockSaver(const BlockManager* manager, const ConfigSPtr& config):
  _manager(manager) {
}

BlockSaver::~BlockSaver() {
}

BlockID BlockSaver::saveBlock(const BlockSPtr& block) {
  const BlockResolverSPtr resolver = _manager->getBlockResolver();
  BlockID newLocalBID = block->getID();
  if (0 == newLocalBID) {
    newLocalBID = resolver->newLocalID();
    log->debug("saveBlock request newLocalID: %d\n", newLocalBID);
  }
  // get a new ID for file name
  string path = _manager->convertBlockIDToPath(newLocalBID);

  //create new file, filename is "locatDir+newLocalBID"
  int file = open(
    path.data(),
    O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  if (file <= 0){
    log->error("create file failed: %s\n", path.data());
    return 0;
  } else {
    write(file, block->_data, Block::blockSize);
    //log->debug("write data: %s\n", block->_data);
    close(file);
    resolver->addLocalID(newLocalBID);
    block->setID(newLocalBID);
    return newLocalBID;
  }
}

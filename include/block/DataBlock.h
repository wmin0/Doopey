#ifndef _DOOPEY_DATA_BLOCK_H_
#define _DOOPEY_DATA_BLOCK_H_

#include "block/Block.h"
#include "common/Doopey.h"

#include <memory>

using std::shared_ptr;

namespace Doopey {

  class OperationDelegate;

  class DataBlock: public Block {
    public:
      DataBlock(unsigned char* data, BlockID id): Block(data, id) {}
      virtual ~DataBlock() {}

  }; // class Block

};  // namespace Doopey

#endif

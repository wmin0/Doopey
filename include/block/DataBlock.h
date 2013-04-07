#ifndef _DOOPEY_DATA_BLOCK_H_
#define _DOOPEY_DATA_BLOCK_H_

#include "block/Block.h"

#include "common/Doopey.h"

namespace Doopey {

  class OperationDelegate;

  class DataBlock: public Block {
    public:
      DataBlock(unsigned char* data, BlockID id): Block(data, id) {}
      virtual ~DataBlock() {}

    public:
      // override declaration
      static OperationDelegate* Delegate;

    private:
      friend class OperationDelegate;
  }; // class Block

  OperationDelegate* DataBlock::Delegate = NULL;

};  // namespace Doopey

#endif

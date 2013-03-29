#ifndef _DOOPEY_META_BLOCK_H_
#define _DOOPEY_META_BLOCK_H_

#include "block/Block.h"

#include <cstdint>

namespace Doopey {

  class OperationDelegate;

  class MetaBlock: public Block {
    public:
      MetaBlock(unsigned char* data, uint64_t id): Block(data, id) {}
      virtual ~MetaBlock() {}

    public:
      // override declaration
      static OperationDelegate* Delegate;

    private:
      friend class OperationDelegate;
  }; // class Block

  OperationDelegate* MetaBlock::Delegate = NULL;

};  // namespace Doopey

#endif

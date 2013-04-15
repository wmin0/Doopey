#ifndef _DOOPEY_META_BLOCK_H_
#define _DOOPEY_META_BLOCK_H_

#include "block/Block.h"
#include "common/Doopey.h"

namespace Doopey {

  class MetaBlock: public Block {
    public:
      MetaBlock(unsigned char* data, BlockID id): Block(data, id) {}
      virtual ~MetaBlock() {}

  }; // class Block


};  // namespace Doopey

#endif

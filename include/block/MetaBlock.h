#ifndef _DOOPEY_META_BLOCK_H_
#define _DOOPEY_META_BLOCK_H_

#include "block/Block.h"
#include "common/Doopey.h"

#include <memory>

using std::shared_ptr;

namespace Doopey {

  class OperationDelegate;

  typedef shared_ptr<OperationDelegate> OperationDelegateSPtr;

  class MetaBlock: public Block {
    public:
      MetaBlock(unsigned char* data, BlockID id): Block(data, id) {}
      virtual ~MetaBlock() {}

    public:
      // override declaration
      static OperationDelegateSPtr delegate;

    private:
      friend class OperationDelegate;
  }; // class Block


};  // namespace Doopey

#endif

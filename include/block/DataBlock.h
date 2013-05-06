#ifndef _DOOPEY_DATA_BLOCK_H_
#define _DOOPEY_DATA_BLOCK_H_

#include "block/Block.h"
#include "common/Doopey.h"

#include <vector>

using std::vector;

namespace Doopey {

  class DataBlock: public Block {
    public:
      DataBlock(unsigned char* data, BlockID id): Block(data, id) {}
      virtual ~DataBlock() {}

      void copyData(const vector<unsigned char>& msgData, size_t size, size_t off);
  }; // class Block

};  // namespace Doopey

#endif

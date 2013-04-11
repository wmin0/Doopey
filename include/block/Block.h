#ifndef _DOOPEY_BLOCK_H_
#define _DOOPEY_BLOCK_H_

#include "common/Doopey.h"

#include <memory>

using std::shared_ptr;

namespace Doopey {

  class Block {
    public:
      Block(unsigned char* data, BlockID id): _data(data), _id(id) {}
      virtual ~Block() {
        // NOTE: mmap when open file and memcpy
        delete[] _data;
      }
    public:
      // initial by block manager
      const static size_t blockSize;
      BlockID getID() const { return _id; }

    protected:
      void setID(BlockID id) { _id = id; }

    protected:
      // NOTICE:
      // data are allocated by loader and then the Block class get the ownership
      // of the pointer.
      // Because of that, we can do mmap by loader, but maybe it's a ill
      // designed protocal.
      unsigned char* _data;
      BlockID _id;

    friend class BlockSaver;
  }; // class Block

};  // namespace Doopey

#endif

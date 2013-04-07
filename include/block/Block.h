#ifndef _DOOPEY_BLOCK_H_
#define _DOOPEY_BLOCK_H_

#include "common/Doopey.h"

namespace Doopey {

  class OperationDelegate;

  class Block {
    public:
      Block(unsigned char* data, BlockID id): _data(data), _id(id) {}
      virtual ~Block() {
        // TODO:
        // munmap(map, FILESIZE)
        delete _data;
      }
    public:
      // initial by block manager
      static OperationDelegate* Delegate;
      const static size_t BlockSize;

    protected:
      // NOTICE:
      // data are allocated by loader and then the Block class get the ownership
      // of the pointer.
      // Because of that, we can do mmap by loader, but maybe it's a ill
      // designed protocal.
      unsigned char* _data;
      BlockID _id;

      friend class OperationDelegate;
  }; // class Block

  OperationDelegate* Block::Delegate = NULL;
  // const size_t Block::BlockSize = 1024 * 64;
  const size_t Block::BlockSize = 1024000 * 64;

};  // namespace Doopey

#endif

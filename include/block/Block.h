#ifndef _DOOPEY_BLOCK_H_
#define _DOOPEY_BLOCK_H_

#include <cstdint>

namespace Doopey {

  class BlockResolver;
  class HeaderDelegate;
//  class DataDelegate;

  class Block {
    public:
      Block(uint64_t id, BlockResolver* resolver):
        _resolver(resolver), _headerDelegate(0), _dataDelegate(0), _id(id)
      {}
      Block(BlockResolver* resolver):
        _resolver(resolver), _headerDelegate(0), _dataDelegate(0), _id(0)
      {}

      virtual ~Block() {}

      virtual const HeaderDelegate* getHeaderDelegate() const = 0;
//      virtual const DataDelegate* getDataDelegate() const = 0;

      virtual void fetch() = 0;
      virtual void save() = 0;

      //static size_t BlockSize = 64M;

    private:
      // id - path resolve and manage
      const BlockResolver* _resolver;

      // read write delegate to prevent over write and update header
      HeaderDelegate* _headerDelegate;
//      DataDelegate* _dataDelegate;
      uint64_t _id;
      unsigned char* _data;

  }; // class BlockBase

};  // namespace Doopey

#endif

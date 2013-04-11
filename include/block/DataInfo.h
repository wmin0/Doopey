#ifndef _DOOPEY_DATA_INFO_H_
#define _DOOPEY_DATA_INFO_H_

#include <memory>

using std::shared_ptr;

namespace Doopey {

  class Block;
  class DataInfo {
    typedef shared_ptr<Block> BlockSPtr;

    public:
      unsigned char* data;
      size_t size;
      ~DataInfo() {}

    private:
      DataInfo() {}
      DataInfo(const BlockSPtr& block, unsigned char* s, size_t len):
        data(s), size(len), _block(block) {}
      // keep reference count
      BlockSPtr _block;
  }; // class DataInfo

};  // namespace Doopey

#endif

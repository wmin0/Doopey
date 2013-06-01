#ifndef _DOOPEY_META_BLOCK_H_
#define _DOOPEY_META_BLOCK_H_

#include <cstdint>

#include "block/Block.h"
#include "common/Doopey.h"

namespace Doopey {

  class MetaBlock: public Block {
    public:
      MetaBlock(unsigned char* data, BlockID id);
      virtual ~MetaBlock() {}

      //Meta struture: name length + name + ctime + number of block + BlockID list
      bool setFileName(string& filename);
      bool setCreateTime(time_t ctime);
      bool setFileSize(uint64_t size);
      bool addDataID(BlockID id);

      string getFileName() const;
      uint64_t getFileSize() const;
      time_t getCreateTime() const;
      uint64_t getDataBlockNumber() const;
      BlockID getDataBlockID(const uint64_t number) const;
    private:
      uint64_t* _nameLength;
      char* _filename;
      time_t* _ctime;
      uint64_t* _size;
      uint64_t* _blockNumber;
      BlockID* _firstBlockID;
  }; // class Block


};  // namespace Doopey

#endif

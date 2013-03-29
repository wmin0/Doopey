#ifndef _DOOPEY_OPERATION_DELEGATE_H_
#define _DOOPEY_OPERATION_DELEGATE_H_

#include <map>
#include <memory>

using std::map;
using std::shared_ptr;

namespace Doopey {

  class Block;
  class DataInfo;

  class OperationDelegate {
    typedef shared_ptr<Block> BlockSPtr;
    typedef shared_ptr<DataInfo> DataInfoSPtr;

    public:
      OperationDelegate() {}
      virtual ~OperationDelegate() {}

      DataInfoWPtr getDataInfoFromBlock(
        const char* name, const BlockSPtr& block) const;

    protected:
      class ColumnLocation {
        public:
          ColumnLocation(size_t o, size_t s): offset(o), size(s) {}
          size_t offset;
          size_t size;
      }; // class ColumnLocation

      map<const char*, ColumnLocation> _locationMap;

  }; // class OperationDelegate

};  // namespace Doopey

#endif

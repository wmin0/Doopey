#ifndef _DOOPEY_OPERATION_DELEGATE_H_
#define _DOOPEY_OPERATION_DELEGATE_H_

#include <map>
#include <memory>
#include <string>

using std::map;
using std::shared_ptr;
using std::string;

namespace Doopey {

  class Block;
  class DataInfo;

  typedef shared_ptr<DataInfo> DataInfoSPtr;

  class OperationDelegate {
    typedef shared_ptr<Block> BlockSPtr;

    public:
      OperationDelegate() {}
      virtual ~OperationDelegate() {}

      DataInfoSPtr getDataInfoFromBlock(
        const char* name, const BlockSPtr& block) const;

    protected:
      class ColumnLocation {
        public:
          ColumnLocation(size_t o, size_t s): offset(o), size(s) {}
          size_t offset;
          size_t size;
      }; // class ColumnLocation

      map<string, ColumnLocation> _locationMap;

  }; // class OperationDelegate

};  // namespace Doopey

#endif

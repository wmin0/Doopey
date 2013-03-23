#ifndef _DOOPEY_OPERATION_DELEGATE_H_
#define _DOOPEY_OPERATION_DELEGATE_H_

#include <map>

namespace Doopey {

  class Block;
  class DataInfo;

  class OperationDelegate {
    using std::map;

    public:
      OperationDelegate() {}
      virtual ~OperationDelegate() {}

      DataInfo* getDataInfoFromBlock(
        const char* name, const Block* block) const;

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

#include "block/OperationDelegate.h"

#include "block/Block.h"
#include "block/DataInfo.h"

#include <string>

using namespace Doopey;

using std::string;

DataInfoSPtr OperationDelegate::getDataInfoFromBlock(
  const char* name, const BlockSPtr& block) const {
  map<string, ColumnLocation>::const_iterator it = _locationMap.find(name);
  if (_locationMap.end() == it) {
    return DataInfoSPtr(NULL);
  }
  return DataInfoSPtr(new DataInfo(block, block->_data + it->second.offset, it->second.size));
}

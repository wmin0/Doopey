#include "block/DataDelegate.h"

using namespace Doopey;

DataDelegate::DataDelegate() {
  _locationMap = {
    {"type", ColumnLocation(0, 8)}
  };
}

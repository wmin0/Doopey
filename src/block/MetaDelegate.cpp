#include "block/MetaDelegate.h"

using namespace Doopey;

MetaDelegate::MetaDelegate() {
  _locationMap = {
    {"type", ColumnLocation(0, 8)}
  };
}

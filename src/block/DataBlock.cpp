#include "block/DataBlock.h"

#include <cstring>
#include <vector>

using namespace std;
using namespace Doopey;

void DataBlock::copyData(const vector<unsigned char>& msgData){
  memcpy(_data, msgData.data(),msgData.size());
}


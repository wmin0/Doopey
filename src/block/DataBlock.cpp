#include "block/DataBlock.h"

#include <cstring>
#include <vector>

using namespace std;
using namespace Doopey;

void DataBlock::copyData(const vector<unsigned char> msgData, size_t off, size_t size){
  memcpy(_data, msgData.data()+off, size);
}


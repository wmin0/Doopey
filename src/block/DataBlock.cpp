#include "block/DataBlock.h"

#include <cstring>
#include <vector>

using namespace std;
using namespace Doopey;

bool DataBlock::copyData(const vector<unsigned char>& msgData){
  if(msgData.size() > DataBlock::blockSize)
    return false;
  else
    memcpy(_data, msgData.data(), msgData.size());
  return true;
}


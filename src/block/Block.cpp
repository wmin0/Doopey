#include "block/Block.h"

using namespace Doopey;

OperationDelegateSPtr Block::delegate;
// const size_t Block::BlockSize = 1024 * 64;
const size_t Block::blockSize = 1024000 * 64;


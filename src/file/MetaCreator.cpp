#include "file/MetaCreator.h"
#include "block/MetaBlock.h"

using namespace Doopey;

typedef shared_ptr<MetaBlock> MetaBlockSPtr;

MetaCreator::MetaCreator()
{
}

MetaCreator::~MetaCreator()
{
}

MetaBlockSPtr MetaCreator::create()
{
  return NULL;
}

#include "config/ConfigLoader.h"

#include "config/SectionCollection.h"

using namespace Doopey;

SectionCollectionSPtr ConfigLoader::loadConfig(const char* path) {
  return SectionCollectionSPtr(new SectionCollection());
}

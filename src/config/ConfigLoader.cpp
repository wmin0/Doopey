#include "config/ConfigLoader.h"

#include "config/SectionCollection.h"

using namespace Doopey;

SectionCollection* ConfigLoader::loadConfig(const char* path) {
  return new SectionCollection();
}

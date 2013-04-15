#include "common/SectionCollection.h"
#include "common/Doopey.h"

using namespace Doopey;
using namespace std;

ConfigSPtr SectionCollection::getConfig(const char* section) const{
  ConfigSPtr result(NULL);
  string sectionIndex=section;
  map<string, ConfigSPtr>::const_iterator findConfig;
  findConfig = _configTable.find(sectionIndex);

  if(findConfig!= _configTable.end())
    result=findConfig->second;
  else
    log->info("config not found\n");

  return result;
}

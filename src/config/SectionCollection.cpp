#include "config/SectionCollection.h"
#include "logger/Logger.h"

#include <iostream>

using namespace Doopey;
using namespace std;

typedef shared_ptr<Config> ConfigSPtr;

ConfigSPtr SectionCollection::getConfig(const char* section) const{
  ConfigSPtr result(NULL);
  string sectionIndex=section;
  map<string, ConfigSPtr>::const_iterator findConfig;
  findConfig = _configTable.find(sectionIndex);

  if(findConfig!= _configTable.end())
    result=findConfig->second;
  else{
    Logger logger(LL_Info);
    logger.info("config not found");
  }
  return result;
}

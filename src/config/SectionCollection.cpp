#include "config/SectionCollection.h"

#include <iostream>

using namespace Doopey;
using namespace std;

typedef shared_ptr<Config> ConfigSPtr;

ConfigSPtr SectionCollection::getConfig(const char* section){
  ConfigSPtr result;
  string sectionIndex=section;
  map<string, ConfigSPtr>::iterator findConfig;
  findConfig = configTable.find(sectionIndex);

  if(findConfig!=configTable.end())
    result=findConfig->second;
  else cout<<"Config not found"<<endl;

  return result;
}

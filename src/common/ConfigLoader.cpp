#include "common/ConfigLoader.h"
#include "common/Config.h"
#include "common/SectionCollection.h"

#include <fstream>
#include <cstring>
#include <cstdio>

using namespace std;
using namespace Doopey;

SectionCollectionSPtr ConfigLoader::loadConfig(const char* path) {
  string line;
  bool ignoreProperties=false;
  SectionCollectionSPtr sectionCollection(new SectionCollection());
  fstream file;
  file.open(path,ios::in);

  while(getline(file, line)){
    if(line[0]=='[' && line[line.size()-1]==']'){
      ignoreProperties=false;
      char temp[line.size()+1];
      string configName;

      strcpy(temp, line.c_str());
      configName=strtok(temp , "[]");

      ConfigSPtr config(new Config(configName));

      while(getline(file, line) && line.size()>0){
        if(line.find('=')==line.npos){
          log.warning("wrong property format of [%s]\n", configName.c_str());
          continue;
        }

        char temp[line.size()+1];
        strcpy(temp, line.c_str());
        string tempKey,tempValue;
        tempKey=strtok(temp , " =");
        tempValue=strtok(NULL, " =");
        if(config->_values.count(tempKey)>0){
          log.warning("key is pre-existing in [%s]\n", configName.c_str());
          continue;
        }
        config->_values.insert(pair<string, string>(tempKey, tempValue));
      }
      sectionCollection->_configTable.insert(pair<string, ConfigSPtr>(config->_name, config));
    }
    else if(ignoreProperties==false && line.size()>0){
      ignoreProperties=true;
      log.warning("wrong format of config name %s, whose properties are ignored\n", line.c_str());
    }
  }
  file.close();
  return sectionCollection;
}

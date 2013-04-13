#include "config/ConfigLoader.h"
#include "config/Config.h"
#include "config/SectionCollection.h"
#include "logger/Logger.h"

#include <fstream>
#include <cstring>
#include <cstdio>
#include <iostream>

using namespace std;
using namespace Doopey;

typedef shared_ptr<SectionCollection> SectionCollectionSPtr;
typedef shared_ptr<Config> ConfigSPtr;

SectionCollectionSPtr ConfigLoader::loadConfig(const char* path) {
  string line;
  Logger logger(LL_Warning);
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
      configName=strtok(temp , "[");
      configName.erase(configName.size()-1);
//cout<<configName<<endl;
      ConfigSPtr config(new Config(configName));

      while(getline(file, line) && line.size()>0){
        if(line.find('=')==line.npos){
          logger.warning("wrong property format of [%s]\n", configName.c_str());
          continue;
        }

        char temp[line.size()+1];
        strcpy(temp, line.c_str());
        string tempKey,tempValue;
        tempKey=strtok(temp , "=");
        tempValue=strtok(NULL, "=");
        if(config->_values.count(tempKey)>0){
          logger.warning("key is pre-existing\n");
          continue;
        }
        config->_values.insert(pair<string, string>(tempKey, tempValue));
//cout<<tempKey<<" "<<tempValue<<endl;
      }
      sectionCollection->
      _configTable.insert(pair<string, ConfigSPtr>(config->_name, config));
    }
    else if(ignoreProperties==false && line.size()>0){
      ignoreProperties=true;
      logger.warning("wrong format of config name %s, whose properties are ignored\n", line.c_str());
    }
  }

  return sectionCollection;
}

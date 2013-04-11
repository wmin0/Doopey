#include "config/ConfigLoader.h"
#include "config/Config.h"
#include "config/SectionCollection.h"

#include <fstream>
#include <cstring>
#include <cstdio>

using namespace std;
using namespace Doopey;

typedef shared_ptr<SectionCollection> SectionCollectionSPtr;

SectionCollectionSPtr ConfigLoader::loadConfig(const char* path) {
  char line[50];
  SectionCollectionSPtr sectionCollection(new SectionCollection());
  fstream file;
  file.open(path,ios::in);

  while(file.getline(line,sizeof(line))){

    if(line[0]=='['){
      char configName[20];
      for(unsigned int i=0;i<strlen(line)-2;i++) configName[i]=line[i+1];

      configName[strlen(line)-2]='\0';

      shared_ptr<Config> config(new Config(configName));

      while(file.getline(line,sizeof(line)) && strlen(line)>0){
        string tempKey,tempValue;
        tempKey=strtok(line, "=");
        tempValue=strtok(NULL, "=");
        config->values[tempKey]=tempValue;
      }

      sectionCollection->configTable[config->name]=config;
    }
  }

  return sectionCollection;
}

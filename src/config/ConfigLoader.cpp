#include "config/ConfigLoader.h"
#include "config/Config.h"
#include "config/SectionCollection.h"
//#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>

using namespace std;
using namespace Doopey;

typedef shared_ptr<SectionCollection> SectionCollectionSPtr;

SectionCollectionSPtr ConfigLoader::loadConfig(const char* path) {
  char line[20];
  char configName[20];
  char temp[20];

  fstream file;
  file.open(path,ios::in);
  while(file.getline(line,sizeof(line),'\n')){
    if(line[0]=='['){
      //cout<<line<<endl;
      for(unsigned int i=0;i<strlen(line)-2;i++) configName[i]=line[i+1];

      configName[strlen(line)-2]='\0';

      Config config(configName);

      int i=1;
      while(file.getline(line,sizeof(line),'\n') && line[0] != '['){
        sprintf(temp, "%u", i);
        config.values[temp]=line;
        i++;
      }
    }
  }

  return SectionCollectionSPtr(new SectionCollection());
}

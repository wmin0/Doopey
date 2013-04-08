#include "config/Config.h"
#include <map>
#include <iostream>

using namespace Doopey;
using namespace std;

Config::Config(char *configName){
  name = configName;
}

string Config::getValue(const char* key){
  map<const char*, char*>::iterator findValue;
  findValue = values.find(key);
  string result = "";
  if(findValue!=values.end())
    result = findValue->second;
  else cout<<"Value not found"<<endl;

  return result;
}

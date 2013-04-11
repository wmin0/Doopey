#include "config/Config.h"

#include <iostream>

using namespace Doopey;
using namespace std;

Config::Config(string configName){
  name = configName;
}

string Config::getValue(const char* key){
  string keyValue = key;
  map<string, string>::iterator findValue;
  findValue = values.find(keyValue);
  string result = "";
  if(findValue!=values.end())
    result = findValue->second;
  else cout<<"Value not found"<<endl;

  return result;
}

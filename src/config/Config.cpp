#include "config/Config.h"
#include "logger/Logger.h"

#include <iostream>

using namespace Doopey;
using namespace std;

Config::Config(string& configName):_name(configName) {
}

string Config::getValue(const char* key) const{
  string keyValue = key;
  map<string, string>:: const_iterator findValue;
  findValue = _values.find(keyValue);
  string result = "";
  if(findValue!=_values.end())
    result = findValue->second;
  else{
    Logger logger(LL_Info);
    logger.info("value not found");
  }

  return result;
}

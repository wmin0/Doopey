#include "common/Config.h"
#include "common/Doopey.h"

using namespace Doopey;
using namespace std;

Config::Config(const string& configName):_name(configName) {
}

string Config::getValue(const char* key) const{
  string keyValue = key;
  map<string, string>:: const_iterator findValue;
  findValue = _values.find(keyValue);
  string result = "";
  if(findValue!=_values.end())
    result = findValue->second;
  else
    log->info("value not found: %s\n", key);

  return result;
}

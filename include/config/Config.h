#ifndef _DOOPEY_CONFIG_H_
#define _DOOPEY_CONFIG_H_

#include <string>
#include <map>

using std::string;
using std::map;

namespace Doopey {

  class ConfigLoader;
  class SectionCollection;

  class Config {
    typedef map<string, string> MapConfigValue;

    public:
      ~Config() {}
      string getValue(const char* key);
      MapConfigValue values;
      string name;//config name

    private:
      Config(string);

    friend ConfigLoader;
    friend SectionCollection;
  }; // class Config

};  // namespace Doopey

#endif

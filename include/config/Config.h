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
    typedef map<const char*, char*> MAP_CONFIG_VALUE;

    public:
      ~Config() {}
      string getValue(const char* key);
      MAP_CONFIG_VALUE values;
      char *name;//config name

    private:
      Config(char *);

    friend ConfigLoader;
    friend SectionCollection;
  }; // class Config

};  // namespace Doopey

#endif

#ifndef _DOOPEY_CONFIG_H_
#define _DOOPEY_CONFIG_H_

#include <string>
#include <map>

using std::string;
using std::map;

namespace Doopey {

  class Config {
    typedef map<string, string> MapConfigValue;

    public:
      ~Config() {}
      string getValue(const char* key) const;

    private:
      Config(const string&);
      MapConfigValue _values;
      string _name;//config name

    friend class ConfigLoader;
    friend class SectionCollection;
  }; // class Config

};  // namespace Doopey

#endif

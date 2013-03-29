#ifndef _DOOPEY_CONFIG_H_
#define _DOOPEY_CONFIG_H_

#include <string>

using std::string;

namespace Doopey {

  class ConfigLoader;
  class SectionCollection;

  class Config {

    public:
      ~Config() {}
      string getValue(const char* key) { return ""; }

    private:
      Config() {}

    friend ConfigLoader;
    friend SectionCollection;
  }; // class Config

};  // namespace Doopey

#endif

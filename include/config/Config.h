#ifndef _DOOPEY_CONFIG_H_
#define _DOOPEY_CONFIG_H_

#include <string>

using std::string;

namespace Doopey {

  class SectionCollection;
  class ConfigLoader;

  class Config {

    public:
      string getConfig(const char* key) { return ""; }

    private:
      ~Config() {}
      Config() {}

    friend ConfigLoader;
    friend SectionCollection;
  }; // class Config

};  // namespace Doopey

#endif

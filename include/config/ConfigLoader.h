#ifndef _DOOPEY_CONFIG_LOADER_H_
#define _DOOPEY_CONFIG_LOADER_H_

namespace Doopey {

  class Config;
  class SectionCollection;

  class ConfigLoader {

    public:
      ConfigLoader() {}
      ~ConfigLoader() {}

      SectionCollection* loadConfig(const char* path);

    private:
  }; // class ConfigLoader

};  // namespace Doopey

#endif

#ifndef _DOOPEY_CONFIG_LOADER_H_
#define _DOOPEY_CONFIG_LOADER_H_

#include <memory>

using std::shared_ptr;

namespace Doopey {

  class Config;
  class SectionCollection;

  class ConfigLoader {
    typedef shared_ptr<SectionCollection> SectionCollectionSPtr;
    public:
      ConfigLoader() {}
      ~ConfigLoader() {}

      // TODO: consider to be static
      static SectionCollectionSPtr loadConfig(const char* path);

    private:
  }; // class ConfigLoader

};  // namespace Doopey

#endif

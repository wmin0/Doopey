#ifndef _DOOPEY_SECTION_COLLECTION_H_
#define _DOOPEY_SECTION_COLLECTION_H_

#include <memory>

using std::shared_ptr;

namespace Doopey {

  class Config;
  class ConfigLoader;

  class SectionCollection {
    typedef shared_ptr<Config> ConfigSPtr;

    public:
      ~SectionCollection() {}

      ConfigSPtr getConfig(const char* section) { return NULL; }

    private:
      SectionCollection() {}

    friend ConfigLoader;
  }; // class SectionCollection

};  // namespace Doopey

#endif

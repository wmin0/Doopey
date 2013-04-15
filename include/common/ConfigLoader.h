#ifndef _DOOPEY_CONFIG_LOADER_H_
#define _DOOPEY_CONFIG_LOADER_H_

#include "common/Doopey.h"

namespace Doopey {

  class ConfigLoader {
    public:
      ConfigLoader() {}
      ~ConfigLoader() {}

      // TODO: consider to be static
      static SectionCollectionSPtr loadConfig(const char* path);

    private:
  }; // class ConfigLoader

};  // namespace Doopey

#endif

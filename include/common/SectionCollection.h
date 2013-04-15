#ifndef _DOOPEY_SECTION_COLLECTION_H_
#define _DOOPEY_SECTION_COLLECTION_H_

#include "common/Doopey.h"

#include <map>
#include <string>

using std::map;
using std::string;

namespace Doopey {

  class SectionCollection {
    typedef map<string ,ConfigSPtr> MapSectionCollection;
    public:
      ~SectionCollection() {}
      ConfigSPtr getConfig(const char* section) const;

    private:
      SectionCollection() {}
      MapSectionCollection _configTable;

    friend class ConfigLoader;
  }; // class SectionCollection

};  // namespace Doopey

#endif

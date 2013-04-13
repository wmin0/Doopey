#ifndef _DOOPEY_SECTION_COLLECTION_H_
#define _DOOPEY_SECTION_COLLECTION_H_

#include <memory>
#include <map>
#include <string>

using std::shared_ptr;
using std::map;
using std::string;

namespace Doopey {

  class Config;
  class ConfigLoader;

  class SectionCollection {
    typedef shared_ptr<Config> ConfigSPtr;
    typedef map<string ,ConfigSPtr> MapSectionCollection;
    public:
      ~SectionCollection() {}
      ConfigSPtr getConfig(const char* section) const;

    private:
      SectionCollection() {}
      MapSectionCollection _configTable;

    friend ConfigLoader;
  }; // class SectionCollection

};  // namespace Doopey

#endif

#ifndef _DOOPEY_SECTION_COLLECTION_H_
#define _DOOPEY_SECTION_COLLECTION_H_

namespace Doopey {

  class Config;
  class ConfigLoader;

  class SectionCollection {

    public:
      ~SectionCollection() {}

      Config* getConfig(const char* section) { return 0; }

    private:
      SectionCollection() {}

    friend ConfigLoader;
  }; // class SectionCollection

};  // namespace Doopey

#endif

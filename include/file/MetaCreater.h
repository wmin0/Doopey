#ifndef _DOOPEY_META_CREATOR_H_
#define _DOOPEY_META_CREATOR_H_

#include <cstdint>

using std::shared_ptr;

namespace Doopey{

  class MetaBlock;

  class MetaCreator{

    typedef shared_ptr<MetaBlock> MetaBlockSPtr;

    public:
      MetaCreator();
      ~MetaCreator();

      MetaBlockSPtr create();
    private:
      
  };//class MetaCreator

}//namespace Doopey

#endif

#ifndef _DOOPEY_META_DECODER_H_
#define _DOOPEY_META_DECODER_H_

#include <cstdinti>

using std::shared_ptr;

namespace Doopey{

  class MetaBlock;

  class MetaDecoder{

    typedef shared_ptr<MetaBlock> MetaBlockSPtr;
    public:
      MetaDecoder();
      ~MetaDecoder();
      bool decode(const MetaBlockSptr& meta);
    private:

  }//class MetaDecoder

}//namespace Doopey

#endif

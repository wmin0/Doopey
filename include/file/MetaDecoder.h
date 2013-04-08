#ifndef _DOOPEY_META_DECODER_H_
#define _DOOPEY_META_DECODER_H_

#include <cstdint>
#include <memory>

using std::shared_ptr;

namespace Doopey{

  class MetaBlock;

  class MetaDecoder{

    typedef shared_ptr<MetaBlock> MetaBlockSPtr;
    public:
      MetaDecoder();
      ~MetaDecoder();
      bool decode(const MetaBlockSPtr& meta);
    private:

  };//class MetaDecoder

}//namespace Doopey

#endif

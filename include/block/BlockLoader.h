#ifndef _DOOPEY_BLOCK_LOADER_H_
#define _DOOPEY_BLOCK_LOADER_H_

#include "common/Doopey.h"

namespace Doopey {

  class BlockLoader {

    public:
      BlockLoader(const BlockManager* manager, const ConfigSPtr& config);
      ~BlockLoader();
      DataBlockSPtr loadData(const BlockLocationAttrSPtr& attr) const;
      MetaBlockSPtr loadMeta(const BlockLocationAttrSPtr& attr) const;
      DataBlockSPtr newData() const;
      MetaBlockSPtr newMeta() const;

      bool handleRequestBlockData(
        const SocketSPtr& sock, const MessageSPtr& msg);

    private:
      unsigned char* getData(const BlockLocationAttrSPtr& attr) const;
      unsigned char* allocateMem() const;
      const BlockManager* _manager;



  }; // class BlockLoader

};  // namespace Doopey

#endif

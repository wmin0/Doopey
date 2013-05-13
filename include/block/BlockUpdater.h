#ifndef _DOOPEY_BLOCK_UPDATER_H_
#define _DOOPEY_BLOCK_UPDATER_H_

#include "common/Doopey.h"

namespace Doopey {

  class BlockUpdater {
    public:
      BlockUpdater(const BlockManager* manager, const ConfigSPtr& config);
      ~BlockUpdater();

      void updateRemoteBlockInfo(const BlockLocationAttrSPtr& attr);

    private:
      const BlockManager* _manager;
  }; // class BlockUpdater

};  // namespace Doopey

#endif

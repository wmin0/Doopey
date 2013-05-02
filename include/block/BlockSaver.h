#ifndef _DOOPEY_BLOCK_SAVER_H_
#define _DOOPEY_BLOCK_SAVER_H_

#include "common/Doopey.h"

namespace Doopey {

  class Block;
  class BlockManager;
  class Config;

  class BlockSaver {

    public:
      BlockSaver(const BlockManager* manager, const ConfigSPtr& config);
      ~BlockSaver();
      BlockID saveBlock(const BlockSPtr& block);

    private:
      const BlockManager* _manager;

  }; // class BlockSaver

};  // namespace Doopey

#endif

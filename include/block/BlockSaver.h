#ifndef _DOOPEY_BLOCK_SAVER_H_
#define _DOOPEY_BLOCK_SAVER_H_

#include "common/Doopey.h"

#include <memory>
#include <string>

using std::shared_ptr;
using std::string;

namespace Doopey {

  class Block;
  class BlockManager;
  class Config;

  class BlockSaver {
    typedef shared_ptr<Block> BlockSPtr;
    typedef shared_ptr<Config> ConfigSPtr;

    public:
      BlockSaver(const BlockManager* manager, const ConfigSPtr& config);
      ~BlockSaver();
      BlockID saveBlock(const BlockSPtr& block);

    private:
      const BlockManager* _manager;
      string _localDir;

  }; // class BlockSaver

};  // namespace Doopey

#endif

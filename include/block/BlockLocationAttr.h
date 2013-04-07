#ifndef _DOOPEY_BLOCK_LOCATION_ATTR_H_
#define _DOOPEY_BLOCK_LOCATION_ATTR_H_

#include "common/Doopey.h"

#include <ctime>
#include <string>

using std::string;

namespace Doopey {

  class BlockLocationAttr {
    public:
      BlockLocationAttr(): block(0), machine(0), ts(0) {}
      BlockLocationAttr(BlockID b, MachineID m, time_t t):
        block(b), machine(m), ts(t) {}
      void set(BlockID b, MachineID m, time_t t) {
        block = b; machine = m; ts = t;
      }

    public:
      BlockID block;
      MachineID machine;
      // for replacement alg
      time_t ts;
  }; // class BlockLocationAttr

};  // namespace Doopey

#endif

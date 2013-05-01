#ifndef _DOOPEY_BLOCK_LOCATION_ATTR_H_
#define _DOOPEY_BLOCK_LOCATION_ATTR_H_

#include "common/Doopey.h"

#include "block/Block.h"

#include <ctime>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace Doopey {

  enum BlockState {
    BS_Available,
    BS_Invalid,
    BS_Deleting,
  }; // enum BlockState

  class BlockLocationAttr {
    public:
      BlockLocationAttr(): block(0), ts(0), state(BS_Invalid) {}
      BlockLocationAttr(BlockID b, time_t t, BlockState st):
        block(b), ts(t), state(st) {}
      void set(BlockID b, time_t t, BlockState st) {
        block = b; ts = t; state = st;
      }
      bool addMachine(MachineID m);
      void removeMachine(MachineID m);
      // replace m1 to m2
      bool replaceMachine(MachineID m1, MachineID m2);
    public:
      BlockID block;
      vector<MachineID> machine;
      // for replacement alg
      time_t ts;
      BlockState state;
  }; // class BlockLocationAttr

};  // namespace Doopey

#endif

#ifndef _DOOPEY_H_
#define _DOOPEY_H_

#include "logger/Logger.h"

namespace Doopey {

  extern Logger log;

  enum ThreadState {
    Start = 0,
    Run = 1,
    Terminate = 2
  };

};  // namespace Doopey

#endif

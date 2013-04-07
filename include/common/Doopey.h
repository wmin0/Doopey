#ifndef _DOOPEY_H_
#define _DOOPEY_H_

#include "logger/Logger.h"

#include <csignal>
#include <cstdint>

namespace Doopey {

  extern Logger log;

  enum ThreadState {
    TS_None = 0,
    TS_Start = 1,
    TS_Run = 2,
    TS_Terminate = 3
  };

  // signal
  #define SIGRSTOP SIGRTMAX - 1
  #define SIGDSTOP SIGRTMAX - 2
  #define SIGSREQ SIGRTMAX - 3
  #define SIGRREQ SIGRTMAX - 4

  // general type
  typedef uint64_t BlockID;
  typedef uint32_t MachineID;

};  // namespace Doopey

#endif

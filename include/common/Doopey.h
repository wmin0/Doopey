#ifndef _DOOPEY_H_
#define _DOOPEY_H_

#include "logger/Logger.h"

#include <csignal>
#include <cstdint>

namespace Doopey {

  extern Logger log;

  // signal
  #define SIGTSTOP SIGRTMAX - 1
  #define SIGSREQ SIGRTMAX - 2
  #define SIGRREQ SIGRTMAX - 3

  // general type
  typedef uint64_t BlockID;
  typedef uint32_t MachineID;

};  // namespace Doopey

#endif

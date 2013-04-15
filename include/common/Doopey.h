#ifndef _DOOPEY_H_
#define _DOOPEY_H_

#include "logger/Logger.h"

#include <csignal>
#include <cstdint>

namespace Doopey {

  extern Logger log;

  // signal
  #define SIGTSTOP SIGRTMAX - 1
  #define SIGRREQ SIGRTMAX - 2

  // general type
  typedef uint64_t BlockID;
  typedef uint32_t LocalBlockID;
  typedef uint32_t MachineID;

  extern MachineID getMachineIDFromBlockID(BlockID block);
  extern MachineID getLocalIDFromBlockID(BlockID block);
  extern BlockID buildBlockID(MachineID machine, LocalBlockID local);

  extern bool DoopeyInit();
  extern bool DoopeyFinal();

  const int DoopeyPort = 10090;
};  // namespace Doopey

#endif

#ifndef _DOOPEY_H_
#define _DOOPEY_H_

#include "logger/Logger.h"

#include <csignal>

namespace Doopey {

  extern Logger log;

  enum ThreadState {
    Start = 0,
    Run = 1,
    Terminate = 2
  };

  // signal
  #define SIGRSTOP SIGRTMAX - 1
  #define SIGDSTOP SIGRTMAX - 2
  #define SIGSREQ SIGRTMAX - 3
  #define SIGRREQ SIGRTMAX - 4
  #define SIGDREQ SIGRTMAX - 5

};  // namespace Doopey


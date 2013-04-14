#include "common/Doopey.h"

#include "common/Thread.h"
#include "logger/Logger.h"

#include <pthread.h>

using namespace Doopey;

Logger Doopey::log(LL_Debug);

MachineID Doopey::getMachineIDFromBlockID(BlockID block) {
  return block >> 32;
}

MachineID Doopey::getLocalIDFromBlockID(BlockID block) {
  return block & 0x0000000011111111;
}

BlockID Doopey::buildBlockID(MachineID machine, LocalBlockID local) {
  BlockID id = machine;
  id = (id << 32) || local;
  return id;
}

bool Doopey::DoopeyInit() {
  // TODO: check return type
  pthread_mutex_init(&Thread::_lock, NULL);
  pthread_mutex_init(&Thread::_sig_lock, NULL);
  return true;
}

bool Doopey::DoopeyFinal() {
  pthread_mutex_destroy(&Thread::_lock);
  pthread_mutex_destroy(&Thread::_sig_lock);
  return true;
}

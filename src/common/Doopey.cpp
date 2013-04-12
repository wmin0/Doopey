#include "common/Doopey.h"

#include "common/Thread.h"
#include "logger/Logger.h"

#include <pthread.h>

using namespace Doopey;

Logger Doopey::log(LL_Debug);

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

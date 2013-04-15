#include "common/Thread.h"

#include "common/Doopey.h"

#include <csignal>

using namespace Doopey;

Thread* Thread::_this = NULL;
pthread_mutex_t Thread::_lock;

Thread* Thread::_sig_this = NULL;
pthread_mutex_t Thread::_sig_lock;

void* Thread::_stop_arg = NULL;

Thread::Thread(void (*run)(void* arg), void (*stop)(void* arg)):
  _thread(0), _state(TS_None), _run(run), _stop(stop) {
}

Thread::~Thread() {
  stop(NULL);
}

bool Thread::start(void* arg) {
  if (_state == TS_Terminate) {
    log->error("Thread can't be restarted\n");
    return false;
  }
  pthread_mutex_lock(&Thread::_lock);
  Thread::_this = this;
  _state = TS_Start;
  int ret = pthread_create(&_thread, NULL, Thread::threadProc, arg);
  if (0 != ret) {
    log->error("Thread Start Fail: %d\n", ret);
  } else {
    while (_state != TS_Run && _state != TS_Terminate);
  }
  Thread::_this = NULL;
  pthread_mutex_unlock(&Thread::_lock);
  return !ret;
}

bool Thread::stop(void* arg) {
  if (TS_Run != _state) {
    return false;
  }
  pthread_mutex_lock(&Thread::_sig_lock);
  signal(SIGTSTOP, Thread::handleTSTOP);
  Thread::_sig_this = this;
  Thread::_stop_arg = arg;
  pthread_kill(_thread, SIGTSTOP);
  pthread_join(_thread, NULL);
  signal(SIGTSTOP, NULL);
  Thread::_sig_this = NULL;
  Thread::_stop_arg = NULL;
  pthread_mutex_unlock(&Thread::_sig_lock);
  _state = TS_Terminate;
  return true;
}

void Thread::handleTSTOP(int sig) {
  _sig_this->_stop(Thread::_stop_arg);
}

void* Thread::threadProc(void* arg) {
  Thread::_this->_state = TS_Run;
  Thread::_this->_run(arg);
  return NULL;
}

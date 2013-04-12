#ifndef _DOOPEY_THREAD_H_
#define _DOOPEY_THREAD_H_

#include <pthread.h>

namespace Doopey {

  enum ThreadState {
    TS_None = 0,
    TS_Start = 1,
    TS_Run = 2,
    TS_Terminate = 3
  }; // enum ThreadState

  class Thread {

    public:
      Thread(void (*run)(void*), void (*stop)(void*));
      ~Thread();
      bool start(void* arg);
      bool stop(void* arg);
      ThreadState getState() const { return _state; }

    private:
      static Thread* _this;
      static pthread_mutex_t _lock;

      static Thread* _sig_this;
      static pthread_mutex_t _sig_lock;

      static void* _stop_arg;

      static void* threadProc(void* arg);
      static void handleTSTOP(int sig);


      pthread_t _thread;
      ThreadState _state;

      void (*_run)(void*);
      void (*_stop)(void*);

  }; // class Thread

};  // namespace Doopey

#endif

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
      Thread(void (*run)(void* arg), void (*stop)(void* arg));
      virtual ~Thread();
      bool start(void* arg);
      bool stop(void* arg);
      const ThreadState& getState() const { return _state; }

      // init in doopey_init
      static pthread_mutex_t _lock;
      static pthread_mutex_t _sig_lock;
    protected:
      static Thread* _this;

      static Thread* _sig_this;

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

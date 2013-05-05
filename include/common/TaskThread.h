#ifndef _DOOPEY_TASK_THREAD_H_
#define _DOOPEY_TASK_THREAD_H_

#include "common/Thread.h"

#include <pthread.h>

namespace Doopey {

  class TaskThread: public Thread {
    public:
      TaskThread();
      TaskThread(void (*task)(void* obj, void* input, void* output));
      virtual ~TaskThread();

      bool setTask(void (*task)(void* obj, void* input, void* output));
      bool isFree() const { return _free; }
      bool runTask(void* obj, void* input, void* output);

    private:
      static void threadFunc(void* obj);
      static void threadStop(void* obj);
      void mainLoop();

      void init();

      void (*_task)(void*, void*, void*);

      bool _run;
      bool _free;

      void* _task_obj;
      void* _task_input;
      void* _task_output;
      pthread_mutex_t _task_lock;

  }; // class TaskThread

};  // namespace Doopey

#endif

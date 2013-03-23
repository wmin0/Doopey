#ifndef _DOOPEY_DISPATHER_H_
#define _DOOPEY_DISPATHER_H_

#include <pthread.h>

namespace Doopey {

  class Dispatcher {

    public:
      Dispatcher() {}
      ~Dispatcher() {}

    private:
      static void* threadFunc(void* obj) {
        ((Dispatcher*)obj)->mainLoop();
      }
      void mainLoop();

    private:
      pthread_t _thread;

  }; // class Dispatcher

};  // namespace Doopey

#endif

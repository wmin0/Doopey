#ifndef _DOOPEY_DISPATHER_H_
#define _DOOPEY_DISPATHER_H_

#include "common/Doopey.h"

#include <memory>
#include <pthread.h>

using std::shared_ptr;

namespace Doopey {

  class Config;
  class Server;

  class Dispatcher {
    typedef shared_ptr<Config> ConfigSPtr;

    public:
      Dispatcher(const ConfigSPtr& config, const Server* server);
      ~Dispatcher();

      bool start();
      bool stop();

    private:
      static void* threadFunc(void* obj);
      static void handleDSTOP(int sig);

      void* mainLoop();

    private:
      static Dispatcher* _this;

      // do not delete it!!
      const Server* _server;

      pthread_t _thread;
      ThreadState _threadState;

  }; // class Dispatcher

};  // namespace Doopey

#endif

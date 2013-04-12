#ifndef _DOOPEY_DISPATHER_H_
#define _DOOPEY_DISPATHER_H_

#include "common/Doopey.h"

#include <memory>

using std::shared_ptr;

namespace Doopey {

  class Config;
  class Thread;
  class Server;

  class Dispatcher {
    typedef shared_ptr<Config> ConfigSPtr;
    typedef shared_ptr<Thread> ThreadSPtr;

    public:
      Dispatcher(const Server* server, const ConfigSPtr& config);
      ~Dispatcher();

      bool start();
      bool stop();

    private:
      static void threadFunc(void* obj);
      static void threadStop(void* obj);

      void mainLoop();

    private:
      // do not delete it!!
      const Server* _server;

      ThreadSPtr _thread;

      bool _run;

  }; // class Dispatcher

};  // namespace Doopey

#endif

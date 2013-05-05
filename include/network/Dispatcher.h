#ifndef _DOOPEY_DISPATHER_H_
#define _DOOPEY_DISPATHER_H_

#include "common/Doopey.h"

#include <vector>

using std::vector;

namespace Doopey {

  class Dispatcher {
    public:
      Dispatcher(Server* server, const ConfigSPtr& config);
      ~Dispatcher();

      bool start();
      bool stop();

    private:
      static void threadFunc(void* obj);
      static void threadStop(void* obj);
      static void dispatch(void* dispatch, void* sock, void* output);

      void mainLoop();

    private:
      // do not delete it!!
      Server* _server;

      ThreadSPtr _thread;

      bool _run;
      vector <TaskThreadSPtr> _threadPool;

      SocketSPtr _socket;

      size_t _threadNum;
  }; // class Dispatcher

};  // namespace Doopey

#endif

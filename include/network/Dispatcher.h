#ifndef _DOOPEY_DISPATHER_H_
#define _DOOPEY_DISPATHER_H_

#include "common/Doopey.h"

#include <vector>
#include <memory>

using std::shared_ptr;
using std::vector;

namespace Doopey {

  class Config;
  class TaskThread;
  class Thread;
  class Socket;
  class Server;

  class Dispatcher {
    typedef shared_ptr<Config> ConfigSPtr;
    typedef shared_ptr<Thread> ThreadSPtr;
    typedef shared_ptr<TaskThread> TaskThreadSPtr;
    typedef shared_ptr<Socket> SocketSPtr;

    public:
      Dispatcher(const Server* server, const ConfigSPtr& config);
      ~Dispatcher();

      bool start();
      bool stop();

    private:
      static void threadFunc(void* obj);
      static void threadStop(void* obj);
      static void dispatch(void* dispatch, void* sock);

      void mainLoop();

    private:
      // do not delete it!!
      const Server* _server;

      ThreadSPtr _thread;

      bool _run;
      vector <TaskThreadSPtr> _threadPool;

      SocketSPtr _socket;

      size_t _threadNum;
  }; // class Dispatcher

};  // namespace Doopey

#endif

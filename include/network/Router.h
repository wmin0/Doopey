#ifndef _DOOPEY_ROUTER_H_
#define _DOOPEY_ROUTER_H_

#include "common/Doopey.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>

using std::map;
using std::shared_ptr;
using std::string;

namespace Doopey {

  class Config;
  class Server;
  class Thread;

  class Router {
    typedef shared_ptr<Config> ConfigSPtr;
    typedef shared_ptr<Thread> ThreadSPtr;

    public:
      Router(const Server* server, const ConfigSPtr& config);
      ~Router();

      bool start();
      bool stop();

      // putRequest(class)


    private:
      static void threadFunc(void* obj);
      static void threadStop(void* obj);

      static void handleRREQ(int sig);

      void mainLoop();

    private:
      static Router* _this;

      const Server* _server;

      ThreadSPtr _thread;
      bool _run;

      map<uint64_t, string> _routingTable;
      // queue requestQueue;
  }; // class Router

};  // namespace Doopey

#endif

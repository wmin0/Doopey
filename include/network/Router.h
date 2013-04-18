#ifndef _DOOPEY_ROUTER_H_
#define _DOOPEY_ROUTER_H_

#include "common/Doopey.h"

#include <cstdint>
#include <map>
#include <string>

using std::map;
using std::string;

namespace Doopey {

  class Router {

    public:
      Router(const Server* server, const ConfigSPtr& config);
      ~Router();

      bool start();
      bool stop();

      // putRequest(class)


    private:
      static void threadFunc(void* obj);
      static void threadStop(void* obj);

      void mainLoop();

    private:
      static Router* _this;
      // second
      const static uint32_t heartBeatInterval;

      const Server* _server;

      ThreadSPtr _thread;
      bool _run;

      map<uint64_t, string> _routingTable;


  }; // class Router

};  // namespace Doopey

#endif

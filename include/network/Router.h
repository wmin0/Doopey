#ifndef _DOOPEY_ROUTER_H_
#define _DOOPEY_ROUTER_H_

#include "common/Doopey.h"

#include <cstdint>
#include <map>
#include <memory>
#include <pthread.h>
#include <string>

using std::map;
using std::shared_ptr;
using std::string;

namespace Doopey {

  class Config;

  class Router {
    typedef shared_ptr<Config> ConfigSPtr;

    public:
      Router(const ConfigSPtr& config);
      ~Router();

      // we need pass this to pthread, so we can't do so in constructor
      bool start();
      bool stop();

      // putRequest(class)


    private:
      static void* threadFunc(void* obj);
      static void handleRSTOP(int sig);
      static void handleRREQ(int sig);

      void* mainLoop();

    private:
      static Router* _this;

      pthread_t _thread;
      ThreadState _threadState;

      map<uint64_t, string> _routingTable;
      // queue requestQueue;
  }; // class Router

};  // namespace Doopey

#endif

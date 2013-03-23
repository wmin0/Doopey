#ifndef _DOOPEY_ROUTER_H_
#define _DOOPEY_ROUTER_H_

#include <pthread.h>
#include <cstdint>
#include <map>
#include <string>

namespace Doopey {

  class Router {
    using std::map;
    using std::string;

    public:
      Router() {}
      ~Router() {}

      // putRequest(class)

      // startThread();

    private:
      static void* threadFunc(void* obj) {
        ((Router*)obj)->mainLoop();
      }
      void mainLoop();

    private:
      pthread_t _thread;
      map<uint64_t, string> _routingTable;
      // queue requestQueue;
  }; // class Router

};  // namespace Doopey

#endif

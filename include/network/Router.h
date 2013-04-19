#ifndef _DOOPEY_ROUTER_H_
#define _DOOPEY_ROUTER_H_

#include "common/Doopey.h"

#include <cstdint>
#include <map>
#include <string>

using std::pair;
using std::map;
using std::string;

namespace Doopey {

  class Router {
    typedef pair<MachineID, string> RoutingPair;
    typedef map<MachineID, string> RoutingMap;
    public:
      Router(const Server* server, const ConfigSPtr& config);
      ~Router();

      bool start();
      bool stop();

      void request(const MessageSPtr& msg, const SocketSPtr& sock);

      // routing operation functions
      bool addRoutingPath(const MachineID id, const string& ip);
      
      // handle request functions
      bool handleNeighborInit(const SocketSPtr& sock);

    private:
      static void threadFunc(void* obj);
      static void threadStop(void* obj);

      void mainLoop();

      bool initTopology(const string& list);
      bool initConnectNeignbor(const string& ip);
    private:
      static Router* _this;
      // second
      const static uint32_t heartBeatInterval;

      const Server* _server;

      ThreadSPtr _thread;
      bool _run;

      RoutingMap _routingTable;


  }; // class Router

};  // namespace Doopey

#endif

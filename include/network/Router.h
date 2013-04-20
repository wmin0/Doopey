#ifndef _DOOPEY_ROUTER_H_
#define _DOOPEY_ROUTER_H_

#include "common/Doopey.h"

#include <cstdint>
#include <map>
#include <string>
#include <set>

using std::pair;
using std::map;
using std::set;
using std::string;

namespace Doopey {

  class RoutingEntry {
    public:
      RoutingEntry(): ip(""), d(0) {}
      RoutingEntry(const string& ip, const uint16_t& d):
        ip(ip), d(d) {}
      string ip;
      uint16_t d;
  }; // class RoutingEntry

  class Router {
    typedef pair<MachineID, RoutingEntry> RoutingPair;
    typedef map<MachineID, RoutingEntry> RoutingMap;
    typedef set<MachineID> MachineIDSet;
    public:
      Router(Server* server, const ConfigSPtr& config);
      ~Router();

      bool start();
      bool stop();

      void broadcast(const MessageSPtr& msg) const;
      bool sendTo(MachineID id, const MessageSPtr& msg) const;

      void request(const MessageSPtr& msg, const SocketSPtr& sock);

    private:
      // routing operation functions
      bool addRoutingPath(
        const MachineID id, const string& ip, const uint16_t& d);
      // handle request functions
      bool handleNeighborInit(const SocketSPtr& sock);
      bool handleMachineIDMax(const SocketSPtr& sock);
      bool handleUpdateMachineIDMax(const MessageSPtr& msg);


    private:
      static void threadFunc(void* obj);
      static void threadStop(void* obj);

      void mainLoop();

      bool initTopology(const string& list);
      bool initConnectNeighbor(const string& ip);
      void initMachineID();
    private:
      static Router* _this;
      // second
      const static uint32_t heartBeatInterval;

      Server* _server;

      ThreadSPtr _thread;
      bool _run;

      RoutingMap _routingTable;
      MachineIDSet _neighbors;

  }; // class Router

};  // namespace Doopey

#endif

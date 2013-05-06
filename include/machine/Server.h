#ifndef _DOOPEY_SERVER_H_
#define _DOOPEY_SERVER_H_

#include "common/Doopey.h"

#include <string>
#include <pthread.h>

using std::string;

namespace Doopey {

  class Server {

    public:
      Server(const SectionCollectionSPtr& section);
      ~Server();
      bool start();

      const BlockManagerSPtr& getBlockManager() const { return _blockManager; }
      const RouterSPtr& getRouter() const { return _router; }
      const FileManagerSPtr& getFileManager() const { return _fileManager; }
      const MachineID& getMachineID() const { return _machineID; }
      void setMachineID(const MachineID& id) { _machineID = id; }
      const MachineID& getMachineIDMax() const { return _machineIDMax; }
      void setMachineIDMax(const MachineID& id) { _machineIDMax = id; }
      const string& getLocalIP() const { return _ip; }
      void serve();

      void request(const MessageSPtr& msg, const SocketSPtr& sock);
      //int getSysInfoFreeMemInKB(void);
      bool getSysInfoAvailDisk(const SocketSPtr&);
    private:
      static void handleTERM(int sig);
      static void handleINT(int sig);
      // runtime testing function
      static void handleUSR1(int sig);
      static void signalStop();

      void attachSignal();
      void detachSignal();
      void setupLocalIP();

      void loadSnapshot();
      void saveSnapshot();

    private:
      static Server* _this;

      SectionCollectionSPtr _sectionCollection;

      MachineID _machineID;
      MachineID _machineIDMax;
      string _ip;

      BlockManagerSPtr _blockManager;
      DispatcherSPtr _dispatcher;
      RouterSPtr _router;
      FileManagerSPtr _fileManager;

      // wait lock
      pthread_mutex_t _mutex;

      //char line [256];

  }; // class Server

};  // namespace Doopey

#endif

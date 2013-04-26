#ifndef _DOOPEY_SERVER_SNAPSHOT_H_
#define _DOOPEY_SERVER_SNAPSHOT_H_

#include "common/Doopey.h"

namespace Doopey {

  class ServerSnapshot {
    public:
      ServerSnapshot();
      ~ServerSnapshot();
      bool load();
      bool save();
      void setMachineID(const MachineID& id) { _machineID = id; }
      const MachineID& getMachineID() const { return _machineID; }
    private:
      static const char* _filepath;
    private:
      MachineID _machineID;

  }; // class ServerSnapshot

};  // namespace Doopey

#endif

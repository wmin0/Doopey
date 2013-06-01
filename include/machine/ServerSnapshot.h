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
      void setFileTree(const string& tree) { _dumpFileTree = tree;}
      const MachineID& getMachineID() const { return _machineID; }
      const string& getFileTree() const { return _dumpFileTree; }
    private:
      static const char* _dirpath;
      static const char* _filepath;
    private:
      MachineID _machineID;
      string _dumpFileTree;

      void checkDirExist();

  }; // class ServerSnapshot

};  // namespace Doopey

#endif

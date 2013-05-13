#ifndef _DOOPEY_MESSAGE_H_
#define _DOOPEY_MESSAGE_H_

#include "common/Doopey.h"

#include <unistd.h>
#include <vector>

using std::vector;

namespace Doopey {

  enum MessageType {
    MT_None    = 0,
    MT_File    = 1,
    MT_Router  = 2,
    MT_Block   = 3,
    MT_Machine = 4,
  }; // enum MessageType

  enum MessageCmd {
    MC_None = 0,
    // Command used by FileManager ---------------------------------------------
    // means the first message in file uploading
    MC_UpFileStart,
    // means data in msg is meta infomation
    MC_UpFileMeta,
    // means data in msg is the data
    MC_UpFileData,
    // means the last message in file uploading
    MC_UpFileEnd,
    // means client want to get file
    MC_RequestFile,
    // means client want to see the list in the path he requst
    MC_RequestList,
    // -------------------------------------------------------------------------
    // Command used by Router --------------------------------------------------
    // Common Router ACK
    MC_RouterACK,
    // init msg to declare neighborhood
    MC_NeighborInit,
    // request MachineIDMax
    MC_MachineIDMax,
    // update MachineIDMax
    MC_UpdateMachineIDMax,
    // request routing table
    MC_RequestRoutingTable,
    // check repeat id
    MC_CheckRepeatMachineID,
    // -------------------------------------------------------------------------
    // Command used by Block ---------------------------------------------------
    // Common Block ACK
    MC_BlockACK,
    // Ask for block location
    MC_RequestBlockLocation,
    // ACK for MC_RequestBlockLocation
    MC_RequestBlockLocationACK,
    // Request remote block data
    MC_RequestBlockData,
    // Check Remote Block Alive
    MC_CheckBlockAlive,
    // Request Do Replica,
    MC_DoReplica,
    // Update Replica
    MC_UpdateReplica,
    // Remote Copy Block Request
    MC_CopyBlockFromRemote,
    //--------------------------------------------------------------------------
    // Command used by Machine -------------------------------------------------
    // ACK for RequestSysInfo
    MC_SysInfoACK,
    // Request MEM of system info
    MC_RequestSysInfoMem,
    // Request Free Space of System info
    MC_RequestSysInfoDisk,
  }; // enum MessageCmd

  class Message {
    public:
      Message(MessageType type, MessageCmd cmd);
      // deserialize
      Message(const vector<unsigned char>& data);
      ~Message();

      bool addData(const unsigned char* data, size_t s, size_t len);

      bool serilize(vector<unsigned char>& output) const;

      const MessageType& getType() const { return _type; }
      const MessageCmd& getCmd() const { return _cmd; }
      const vector<unsigned char>& getData() const { return _data; }
      const MachineID& getSrc() const { return _src; }
      const MachineID& getDest() const { return _dest; }
      const uint32_t& getHash() const { return _hash; }
    private:
      void setSrc(const MachineID& id) { _src = id; }
      void setDest(const MachineID& id) { _dest = id; }
      void setHash(const uint32_t& hash) { _hash = hash; }

    private:
      MessageType _type;
      MessageCmd _cmd;
      MachineID _src;
      MachineID _dest;
      uint32_t _hash;
      vector<unsigned char> _data;
    friend class SocketTest;
    friend class Router;
  }; // class Message

};  // namespace Doopey

#endif

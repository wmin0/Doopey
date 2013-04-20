#ifndef _DOOPEY_MESSAGE_H_
#define _DOOPEY_MESSAGE_H_

#include "common/Doopey.h"

#include <unistd.h>
#include <vector>

using std::vector;

namespace Doopey {

  enum MessageType {
    MT_None   = 0,
    MT_File   = 1,
    MT_Router = 2,
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
    // -------------------------------------------------------------------------

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

    private:
      MessageType _type;
      MessageCmd _cmd;
      vector<unsigned char> _data;
    friend class SocketTest;
  }; // class Message

};  // namespace Doopey

#endif

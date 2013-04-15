#ifndef _DOOPEY_MESSAGE_H_
#define _DOOPEY_MESSAGE_H_

#include "common/Doopey.h"

#include <unistd.h>
#include <vector>

using std::vector;

namespace Doopey {

  enum MessageType {
    MT_None = 0,
    MT_File = 1,
  }; // enum MessageType

  enum MessageCmd {
    MC_None = 0,
    //Command used by FileManager ------------------
    MC_FileMeta = 1,
    MC_FileData = 2,
    MC_FileEnd = 3,
    //---------------------------------------------
  }; // enum MessageCmd

  class Message {
    public:
      Message(MessageType type, MessageCmd cmd);
      // deserialize
      Message(const vector<unsigned char>& data);
      ~Message();

      bool addData(const unsigned char* data, size_t s, size_t len);

      bool serilize(vector<unsigned char>& output) const;

      MessageType getType() const { return _type; }
      MessageCmd getCmd() const { return _cmd; }

      vector<unsigned char> getData() const { return _data; }

    private:
      MessageType _type;
      MessageCmd _cmd;
      vector<unsigned char> _data;
    friend class SocketTest;
  }; // class Message

};  // namespace Doopey

#endif

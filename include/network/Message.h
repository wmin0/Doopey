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
    //Command used by FileManager ---------------------------------------------
    MC_UpFileStart = 1,//means the first message in file uploading
    MC_UpFileMeta = 2, //means data in msg is meta information
    MC_UpFileData = 3, //means data in msg is the data
    MC_UpFileEnd = 4,  //means the last message in file uploading
    MC_RequestFile = 5,//means client want to get file
    MC_RequestList = 6,//means client want to see the list in the path he request
    //-------------------------------------------------------------------------
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

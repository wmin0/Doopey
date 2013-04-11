#ifndef _DOOPEY_MESSAGE_H_
#define _DOOPEY_MESSAGE_H_

#include <vector>
#include <memory>

using std::shared_ptr;
using std::vector;

namespace Doopey {

  enum MessageType {
    MT_None = 0,
  }; // enum MessageType

  enum MessageCmd {
    MC_None = 0,
  }; // enum MessageCmd

  class Message {
    public:
      Message(MessageType type, MessageCmd cmd);
      // deserialize
      Message(const vector<unsigned char>& data);
      ~Message();

      bool addData(const unsigned char* data, size_t s, size_t len);

      bool serilize(vector<unsigned char>& output) const;

    private:
      MessageType _type;
      MessageCmd _cmd;
      vector<unsigned char> _data;
    friend class SocketTest;
  }; // class Message

};  // namespace Doopey

#endif

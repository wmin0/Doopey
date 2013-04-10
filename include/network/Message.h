#ifndef _DOOPEY_MESSAGE_H_
#define _DOOPEY_MESSAGE_H_

#include <vector>
#include <memory>

using std::shared_ptr;
using std::vector;

namespace Doopey {

  enum MessageType {
  }; // enum MessageType

  enum MessageCmd {
  }; // enum MessageCmd

  class Message {
    public:
      Message(MessageType type, MessageCmd cmd);
      // deserialize
      Message(const vector<unsigned char>& data);
      ~Message();

      bool addData(const unsigned char* data, size_t s, size_t len);

      vector<unsigned char> serilize();

    private:
      MessageType _type;
      MessageCmd _cmd;
      vector _data;
  }; // class Message

};  // namespace Doopey

#endif

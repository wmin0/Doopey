#ifndef _DOOPEY_MESSAGE_H_
#define _DOOPEY_MESSAGE_H_

#include <memory>

using std::shared_ptr;

namespace Doopey {

  enum MessageType {
  }; // enum MessageType

  enum MessageCmd {
  }; // enum MessageCmd

  class Message {
    typedef shared_ptr<unsigned char> UCharSPtr;
    public:
      Message(MessageType type, MessageCmd cmd, size_t size);
      // deserialize
      Message(const UCharSPtr& data);
      ~Message();

      bool addData(const UCharSPtr& data, size_t s, size_t len);
      bool addData(const unsigned char* data, size_t s, size_t len);

      UCharSPtr serilize();

    private:
      MessageType _type;
      MessageCmd _cmd;
      UCharSPtr _data;
      size_t _size;
  }; // class Message

};  // namespace Doopey

#endif

#ifndef _DOOPEY_MESSAGE_H_
#define _DOOPEY_MESSAGE_H_

namespace Doopey {

  class Message {
    public:
      Message() {}
      ~Message() {}

      unsigned char* serilize();
      void deserilize(unsigned char* data);

    private:

  }; // class Message

};  // namespace Doopey

#endif

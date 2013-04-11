#ifndef _DOOPEY_SOCKET_H_
#define _DOOPEY_SOCKET_H_

#include <memory>

using std::shared_ptr;

namespace Doopey {

  enum SocketType {
    ST_TCP = 0,
    ST_UDP = 1
  }; // enum SocketType

  class Message;

  class Socket {
    typedef shared_ptr<Socket> SocketSPtr;
    typedef shared_ptr<Message> MessageSPtr;

    public:
      Socket(SocketType type);
      ~Socket();
      bool connect(const char* servername, int port);
      bool bind(int port);
      bool listen();
      SocketSPtr accept();

      bool send(const MessageSPtr& msg);
      MessageSPtr recieve();
      // TODO: complete UDP later
      // bool sendTo

      bool isConnected() const { return _isConnected; }

    private:
      Socket(SocketType type, int fd, bool conn):
        _type(type), _fd(fd), _isConnected(conn) {}

    private:
      // 64k
      static const size_t sliceSize = 65536;

    private:
      SocketType _type;
      int _fd;
      bool _isConnected;



  }; // class Socket

};  // namespace Doopey

#endif

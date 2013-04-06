#ifndef _DOOPEY_SOCKET_H_
#define _DOOPEY_SOCKET_H_

#include <memory>

using std::shared_ptr;

namespace Doopey {

  enum SocketType {
    ST_TCP = 0,
    ST_UDP = 1
  }; // enum SocketType

  class Socket {
    typedef shared_ptr<Socket> SocketSPtr;

    public:
      Socket(SocketType type);
      ~Socket();
      bool connect(const char* servername, int port);
      bool bind(int port);
      bool listen();
      SocketSPtr accept();

    private:
      SocketType _type;
      int _fd;

  }; // class Socket

};  // namespace Doopey

#endif

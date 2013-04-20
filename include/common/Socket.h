#ifndef _DOOPEY_SOCKET_H_
#define _DOOPEY_SOCKET_H_

#include "common/Doopey.h"

#include <pthread.h>
#include <string>

using std::string;

namespace Doopey {

  enum SocketType {
    ST_TCP = 0,
    ST_UDP = 1
  }; // enum SocketType

  class Message;

  class Socket {

    public:
      Socket(SocketType type);
      ~Socket();
      bool connect(const string& servername, int port);
      bool connect(const char* servername, int port);
      bool bind(int port);
      bool listen();
      SocketSPtr accept();

      bool send(const MessageSPtr& msg);
      MessageSPtr receive();
      // TODO: complete UDP later
      // bool sendTo

      bool isConnected() const { return _isConnected; }

      void close();

      static pthread_mutex_t _sig_lock;
    private:
      Socket(SocketType type, int fd, bool conn):
        _type(type), _fd(fd), _isConnected(conn) {}

      static Socket* _this;
      static void timeout(int sig);

    private:
      // 64k
      static const size_t sliceSize = 65536;

    private:
      SocketType _type;
      int _fd;
      bool _isConnected;
    friend class SocketTest;
  }; // class Socket

};  // namespace Doopey

#endif

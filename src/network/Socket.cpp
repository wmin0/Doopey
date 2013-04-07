#include "network/Socket.h"
// udp ref: http://www.abc.se/~m6695/udp.html

#include "common/Doopey.h"
#include "network/Message.h"

#include <arpa/inet.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>

using namespace Doopey;

Socket::Socket(SocketType type):
  _type(type), _fd(-1), _isConnected(false) {
  switch (type) {
    case ST_TCP:
      _fd = socket(AF_INET, SOCK_STREAM, 0);
      break;
    case ST_UDP:
      _fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      break;
    default:
      log.error("Unknown SocketType\n");
      break;
  }
}

Socket::~Socket() {
  if (_fd >= 0) {
    close(_fd);
  }
}

bool Socket::connect(const char* servername, int port) {
  if (_fd < 0) {
    log.error("socket open error\n");
    return false;
  }
  if (ST_TCP != _type) {
    log.error("error socket type\n");
    return false;
  }
  struct hostent* server = NULL;
  server = gethostbyname(servername);
  if (NULL == server) {
    log.error("bad server address error\n");
    return false;
  }
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  memcpy(&addr.sin_addr.s_addr,
         server->h_addr_list[0],
         server->h_length);
  if (::connect(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    log.error("socket connect error\n");
    return false;
  }
  _isConnected = true;
  return true;
}

bool Socket::bind(int port) {
  if (_fd < 0) {
    log.error("socket open error\n");
    return false;
  }
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  if (::bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    log.error("socket bind server error\n");
    return false;
  }
  _isConnected = true;
  return true;
}

SocketSPtr Socket::accept() {
  if (_fd < 0) {
    log.error("socket open error\n");
    return SocketSPtr(NULL);
  }
  int conn, conn_len;
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  conn = accept(_fd, (struct sockaddr*)&addr, (socklen_t*)&conn_len);
  if (conn < 0) {
    return SocketSPtr(NULL);
  }
  return SocketSPtr(new Socket(_type, conn, true));
}

bool Socket::send(const MessageSPtr& msg) {
  return false;
}

MessageSPtr Socket::recieve() {
  return MessageSPtr(NULL);
}

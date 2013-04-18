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

#include <cstdint>
#include <vector>

using namespace Doopey;
using std::vector;

typedef shared_ptr<Socket> SocketSPtr;
typedef shared_ptr<Message> MessageSPtr;

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
      log->error("Unknown SocketType\n");
      break;
  }
}

Socket::~Socket() {
  close();
}

void Socket::close() {
  if (_fd >= 0) {
    ::close(_fd);
    _fd = -1;
  }
}

bool Socket::connect(const char* servername, int port) {
  if (_fd < 0) {
    log->error("socket open error\n");
    return false;
  }
  if (ST_TCP != _type) {
    log->error("error socket type\n");
    return false;
  }
  struct hostent* server = NULL;
  server = gethostbyname(servername);
  if (NULL == server) {
    log->error("bad server address error\n");
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
    log->error("socket connect error\n");
    return false;
  }
  _isConnected = true;
  return true;
}

bool Socket::bind(int port) {
  if (_fd < 0) {
    log->error("socket open error\n");
    return false;
  }
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  if (::bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    log->error("socket bind server error\n");
    return false;
  }
  return true;
}

bool Socket::listen() {
  if (::listen(_fd, 1024) < 0) {
    return false;
  }
  _isConnected = true;
  return true;
}

SocketSPtr Socket::accept() {
  if (_fd < 0) {
    log->error("socket open error\n");
    return SocketSPtr(NULL);
  }
  int conn, conn_len = 0;
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  //log->debug("Accepting!\n");
  conn = ::accept(_fd, (struct sockaddr*)&addr, (socklen_t*)&conn_len);
  //log->debug("Accept done %d!\n", conn);
  if (conn < 0) {
    return SocketSPtr(NULL);
  }
  return SocketSPtr(new Socket(_type, conn, true));
}

bool Socket::send(const MessageSPtr& msg) {
  vector<unsigned char> data;
  msg->serilize(data);
  uint64_t len = data.size();
  ssize_t ret = 0;
  uint64_t count = 0;
  //log->debug("send start\n");
  do {
    ret = write(_fd, (unsigned char*)&len + count, sizeof(uint64_t));
    if (0 >= ret) {
      //log->debug("send len fail %d\n", ret);
      _isConnected = false;
      return false;
    }
    //log->debug("send len %d\n", ret);
    count += ret;
  } while (count < sizeof(uint64_t));
  //log->debug("send len %lld done\n", len);
  count = 0;
  do {
    if (count + Socket::sliceSize > len) {
      ret = write(_fd, data.data() + count, len - count);
    } else {
      ret = write(_fd, data.data() + count, Socket::sliceSize);
    }
    if (0 >= ret) {
      //log->debug("send data fail %d\n", ret);
      _isConnected = false;
      return false;
    }
    //log->debug("send data %d\n", ret);
    count += ret;
  } while (count < len);
  //log->debug("send data done\n");
  return true;
}

MessageSPtr Socket::receive() {
  uint64_t len;
  ssize_t ret = 0;
  uint64_t count = 0;
  vector <unsigned char> data;
  // TODO: security and data integrity, but it's the last XD"
  //log->debug("receive start\n");
  do {
    ret = read(_fd, (unsigned char*)&len + count, sizeof(uint64_t));
    if (0 >= ret) {
      //log->debug("len read fail %d\n", ret);
      _isConnected = false;
      return MessageSPtr(NULL);
    }
    //log->debug("read len %d\n", ret);
    count += ret;
  } while (count < sizeof(uint64_t));
  //log->debug("read len done %lld\n", len);
  count = 0;
  data.resize(len);
  do {
    if (count + Socket::sliceSize > len) {
      ret = read(_fd, data.data() + count, len - count);
    } else {
      ret = read(_fd, data.data() + count, Socket::sliceSize);
    }
    if (0 >= ret) {
      //log->debug("data read fail %d\n", ret);
      _isConnected = false;
      return MessageSPtr(NULL);
    }
    //log->debug("read data %d\n", ret);
    count += ret;
  } while (count < len);
  //log->debug("read done\n");
  return MessageSPtr(new Message(data));
}

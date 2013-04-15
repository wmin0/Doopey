#include "network/Message.h"

#include <memory.h>

using namespace Doopey;

Message::Message(MessageType type, MessageCmd cmd):
  _type(type), _cmd(cmd) {
}

Message::Message(const vector<unsigned char>& data) {
  size_t off = 0;
  memcpy(&_type, data.data(), sizeof(MessageType));
  off += sizeof(MessageType);
  memcpy(&_cmd, data.data() + off, sizeof(MessageCmd));
  off += sizeof(MessageCmd);
  _data.resize(data.size() - off);
  memcpy(_data.data(), data.data() + off, data.size() - off);
}

Message::~Message() {
}

bool Message::addData(const unsigned char* data, size_t s, size_t len) {
  if (s + len >= _data.size()) {
    _data.resize(s + len);
  }
  memcpy(_data.data() + s, data, len);
  return true;
}

bool Message::serilize(vector<unsigned char>& output) const {
  output.clear();
  output.resize(
    _data.size() + sizeof(MessageType) + sizeof(MessageCmd));
  size_t off = 0;
  memcpy(output.data(), &_type, sizeof(MessageType));
  off += sizeof(MessageType);
  memcpy(output.data() + off, &_cmd, sizeof(MessageCmd));
  off += sizeof(MessageCmd);
  memcpy(output.data() + off, _data.data(), _data.size());
  return true;
}

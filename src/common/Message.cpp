#include "common/Message.h"

#include <memory.h>

using namespace Doopey;

Message::Message(MessageType type, MessageCmd cmd):
  _type(type), _cmd(cmd), _src(0), _dest(0), _hash(0) {
}

Message::Message(const vector<unsigned char>& data) {
  size_t off = 0;
  memcpy(&_type, data.data(), sizeof(MessageType));
  off += sizeof(MessageType);
  memcpy(&_cmd, data.data() + off, sizeof(MessageCmd));
  off += sizeof(MessageCmd);
  memcpy(&_src, data.data() + off, sizeof(MachineID));
  off += sizeof(MachineID);
  memcpy(&_dest, data.data() + off, sizeof(MachineID));
  off += sizeof(MachineID);
  memcpy(&_hash, data.data() + off, sizeof(uint32_t));
  off += sizeof(uint32_t);
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
    _data.size() +
    sizeof(MessageType) +
    sizeof(MessageCmd) +
    2 * sizeof(MachineID) +
    sizeof(uint32_t));
  size_t off = 0;
  memcpy(output.data(), &_type, sizeof(MessageType));
  off += sizeof(MessageType);
  memcpy(output.data() + off, &_cmd, sizeof(MessageCmd));
  off += sizeof(MessageCmd);
  memcpy(output.data() + off, &_src, sizeof(MachineID));
  off += sizeof(MachineID);
  memcpy(output.data() + off, &_dest, sizeof(MachineID));
  off += sizeof(MachineID);
  memcpy(output.data() + off, &_hash, sizeof(uint32_t));
  off += sizeof(uint32_t);
  memcpy(output.data() + off, _data.data(), _data.size());
  return true;
}

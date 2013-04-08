#include "network/Message.h"

using namespace Doopey;

Message::Message(MessageType type, MessageCmd cmd, size_t size):
  _type(type), _cmd(cmd), _size(size), _data(0) {
  _data = new unsigned char[_size];
}

Message::Message(const UCharSPtr& data) {
}

Message::~Message() {
  delete[] _data;
}

bool Message::addData(const UCharSPtr& data, size_t s, size_t len) {
  return addData(data.get(), s, len);
}

bool Message::addData(const unsigned char* data, size_t s, size_t len) {
  return true;
}

UCharSPtr Message::serilize() {
  return UCharSPtr(NULL);
}

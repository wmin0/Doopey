#include "machine/Client.h"

#include "common/Doopey.h"
#include "common/Socket.h"
#include "common/Message.h"

#include <iostream>
#include <memory>
#include <memory.h>
#include <string>
#include <cstdint>
#include <vector>

#include <getopt.h>

using namespace Doopey;
using namespace std;

typedef shared_ptr<Message> MessageSPtr;

Client::Client() {
}

Client::~Client() {
}

void Client::run(int argc, char** argv) {
  int c = 0;
  struct option long_options[] = {
    {"ls",   no_argument,       0, 'l'},
    {"put",  required_argument, 0, 'p'},
    {"get",  required_argument, 0, 'g'},
    {"help", no_argument,       0, 'h'},
    {0,      0,                 0,  0}
  };
  while (1) {
    int option_index = 0;
    c = getopt_long(argc, argv, "lp:g:h", long_options, &option_index);
    if (-1 == c) {
      break;
    }
    switch (c) {
      // TODO: build option and check one option only
      case 'l':
        cout << "ls request" << endl;
        break;
      case 'p':
        cout << "put request" << endl;
        break;
      case 'g':
        cout << "get request" << endl;
        break;
      case 'h':
        cout << "help request" << endl;
      case '?':
        break;
      default:
        cout << "?? getopt returned character code 0" << c << "??" << endl;
    }
  }
  if (optind < argc) {
    cout << "other arg: ";
    while (optind < argc) {
      cout << argv[optind++] << " ";
    }
    cout << endl;
  }

  //go();
}

void Client::go() {
  Socket socket(ST_TCP);
  if (!socket.connect("localhost", DoopeyPort)) {
    cerr << "connect server error" << endl;
    return;
  }
  MessageSPtr msg(new Message(MT_Machine, MC_RequestSysInfoDisk));
  socket.send(msg);
  MessageSPtr ack = socket.receive();
  string test;
  uint64_t len;
  memcpy(&len, ack->getData().data(), sizeof(uint64_t));
  cout << "len: " << len << endl;
  test.resize(len);
  memcpy(&(test[0]), ack->getData().data() + sizeof(uint64_t), len);
  cout << "test: " << test << endl;
}

bool Client::getFileList(const char* name)
{
  Socket socket(ST_TCP);
  if(!socket.connect("localhost", DoopeyPort)){
    cerr << "connect server error" << endl;
    return 0;
  }
  MessageSPtr msg(new Message(MT_File, MC_RequestList));
  socket.send(msg);
  uint64_t l = strlen(name);
  msg->addData((unsigned char*)&l, 0, sizeof(l));
  msg->addData((unsigned char*)name, 0, l);
  socket.send(msg);
  MessageSPtr reply = socket.receive();
  vector<string>* list = (vector<string>*)msg->getData().data();
  for(vector<string>::iterator it=list->begin(); it<list->end(); it++)
  {
    cout << *it << endl;
  }
  return true;
} 

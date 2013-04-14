#include "machine/Client.h"

#include "network/Socket.h"
#include "network/Message.h"

#include <iostream>
#include <memory>

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

  go(); 
}

void Client::go() {
  Socket socket(ST_TCP);
  if (!socket.connect("localhost", 10090)) {
    cerr << "connect server error" << endl;
    return;
  }
  MessageSPtr msg(new Message(MT_None, MC_None));
  socket.send(msg);
}

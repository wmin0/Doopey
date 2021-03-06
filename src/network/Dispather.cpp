#include "network/Dispatcher.h"

#include "block/BlockManager.h"
#include "common/Doopey.h"
#include "common/Thread.h"
#include "common/TaskThread.h"
#include "logger/Logger.h"
#include "machine/Server.h"
#include "common/Socket.h"
#include "common/Message.h"
#include "network/Router.h"
#include "file/FileManager.h"

#include <pthread.h>
#include <memory>

using namespace Doopey;

typedef shared_ptr<Message> MessageSPtr;

Dispatcher::Dispatcher(Server* server, const ConfigSPtr& config):
  _server(server), _run(false) {
  _socket.reset(new Socket(ST_TCP));
  _thread.reset(new Thread(threadFunc, threadStop));
  // TODO: deciede threadPool num by config
  _threadNum = 4;
  _threadPool.resize(_threadNum);
  for (size_t i = 0; i < _threadNum; ++i) {
    _threadPool[i] = TaskThreadSPtr(new TaskThread(Dispatcher::dispatch));
  }
}

Dispatcher::~Dispatcher() {
  stop();
}

bool Dispatcher::start() {
  log->debug("Dispatcher Thread start!!\n");
  if (!_socket->bind(DoopeyPort) || !_socket->listen()) {
    log->error("Dispatcher Socket bind Err!!\n");
    return false;
  }
  return _thread->start(this);
}

bool Dispatcher::stop() {
  log->debug("Dispatcher Thread stop!!\n");
  return _thread->stop(this);
}

void Dispatcher::threadFunc(void* obj) {
  log->debug("Dispatcher Func!!\n");
  Dispatcher* dispatcher = (Dispatcher*)obj;
  dispatcher->_run = true;
  dispatcher->mainLoop();
}

void Dispatcher::threadStop(void* obj) {
  Dispatcher* dispatcher = (Dispatcher*)obj;
  dispatcher->_run = false;
  dispatcher->_socket->close();
}

void Dispatcher::mainLoop() {
  while (_run) {
    SocketSPtr conn = _socket->accept();
    if (NULL != conn) {
      bool serve = false;
      for (size_t i = 0; i < _threadNum; ++i) {
        if (_threadPool[i]->isFree()) {
          // Note: Hack Orz
          SocketSPtr* ptr = new SocketSPtr(conn);
          if (!_threadPool[i]->runTask(this, ptr, NULL)) {
            delete ptr;
            log->warning("start task thread err\n");
          } else {
            serve = true;
            break;
          }
        }
      }
      if (!serve) {
        log->info("server busy!\n");
        // connect will delete by local variable
      }
    }
  }
}

void Dispatcher::dispatch(void* dispatch, void* sock, void* output) {
  Dispatcher* dispatcher = (Dispatcher*)dispatch;
  SocketSPtr* socket = (SocketSPtr*)sock;
  MessageSPtr msg = (*socket)->receive();
  if (NULL != msg) {
    log->debug("Thread %d: get request from client %d %d\n",
              pthread_self(), msg->getType(), msg->getCmd());
    switch (msg->getType()) {
      case MT_Router:
        dispatcher->_server->getRouter()->request(msg, *socket);
        break;
      case MT_Block:
        dispatcher->_server->getBlockManager()->request(msg, *socket);
        break;
      case MT_Machine:
        dispatcher->_server->request(msg, *socket);
        break;
      case MT_File:
        dispatcher->_server->getFileManager()->receiveQuest(msg, *socket);
        break;
      default:
        break;
    }
  }
  delete socket;
}

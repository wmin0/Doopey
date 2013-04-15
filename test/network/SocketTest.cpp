#include "SocketTest.h"

#include "common/Doopey.h"
#include "common/DoopeyTest.h"
#include "network/Message.h"
#include "network/Socket.h"

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <memory>
#include <pthread.h>
#include <unistd.h>
#include <memory.h>

using namespace std;
using namespace Doopey;

typedef shared_ptr<Message> MessageSPtr;
typedef shared_ptr<Socket> SocketSPtr;

void SocketTest::setUp() {
  server = 0;
  client = 0;
  server_sock = new Socket(ST_TCP);
}

void SocketTest::tearDown() {
  if (0 != client) {
    pthread_join(client, NULL);
  }
  if (0 != server) {
    pthread_join(server, NULL);
  }
  if (NULL != server_sock) {
    delete server_sock;
  }
}

void SocketTest::testSocketConnectTCP() {
  CPPUNIT_ASSERT(true == server_sock->bind(test_port));
  CPPUNIT_ASSERT(true == server_sock->listen());
  CPPUNIT_ASSERT(true == server_sock->isConnected());
  CPPUNIT_ASSERT(
    0 == pthread_create(&server, NULL, SocketTest::testSocketConnectTCPServer, server_sock));
  CPPUNIT_ASSERT(
    0 == pthread_create(&client, NULL, SocketTest::testSocketConnectTCPClient, NULL));
  pthread_join(client, NULL);
  pthread_join(server, NULL);
  client = 0;
  server = 0;
}

void* SocketTest::testSocketConnectTCPServer(void* sock) {
  log.debug("Server Accepting\n");
  Socket* welcome_sock = (Socket*)sock;
  SocketSPtr conn_sock = welcome_sock->accept();
  CPPUNIT_ASSERT(NULL != conn_sock);

  MessageSPtr msg = conn_sock->receive();
  CPPUNIT_ASSERT(NULL != msg);
  unsigned char buf[12];
  memcpy(buf, "Hello World!", 12);
  CPPUNIT_ASSERT(
    0 == memcmp(msg->_data.data(), buf, 12));

  return NULL;
}

void* SocketTest::testSocketConnectTCPClient(void* sock) {
  sleep(3);
  log.debug("Sleep Done\n");
  Socket client_sock(ST_TCP);
  CPPUNIT_ASSERT(true == client_sock.connect("localhost", test_port));
  CPPUNIT_ASSERT(true == client_sock.isConnected());
  MessageSPtr msg(new Message(MT_None, MC_None));
  unsigned char buf[12];
  memcpy(buf, "Hello World!", 12);
  CPPUNIT_ASSERT(true == msg->addData(buf, 0, 12));
  log.debug("%s\n", msg->_data.data());
  CPPUNIT_ASSERT(
    0 == memcmp(msg->_data.data(), buf, 12));
  CPPUNIT_ASSERT(true == client_sock.send(msg));
  return NULL;
}

CppUnit::Test* SocketTest::suite() {
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("SocketTest");
  addUnitTest(suite, SocketTest, testSocketConnectTCP);
  return suite;
}

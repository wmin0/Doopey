#include "SocketTest.h"

#include "common/Doopey.h"
#include "common/DoopeyTest.h"
#include "network/Message.h"
#include "network/Socket.h"

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cstdlib>
#include <memory>
#include <pthread.h>
#include <unistd.h>
#include <memory.h>

using namespace std;
using namespace Doopey;

typedef shared_ptr<Message> MessageSPtr;
typedef shared_ptr<Socket> SocketSPtr;

unsigned char* SocketTest::refData = NULL;

void SocketTest::setUp() {
  server = 0;
  client = 0;
  server_sock = new Socket(ST_TCP);
  log.reset(new Logger(LL_Debug));
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
  refData = new unsigned char[Socket::sliceSize * 2];
  for (size_t i = 0; i < Socket::sliceSize * 2; ++i) {
    refData[i] = rand() % 256;
  }
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
  delete[] refData;
}

void* SocketTest::testSocketConnectTCPServer(void* sock) {
  log->debug("Server Accepting\n");
  Socket* welcome_sock = (Socket*)sock;
  SocketSPtr conn_sock = welcome_sock->accept();
  CPPUNIT_ASSERT(NULL != conn_sock);

  MessageSPtr msg = conn_sock->receive();
  CPPUNIT_ASSERT(NULL != msg);
  unsigned char buf[12];
  memcpy(buf, "Hello World!", 12);
  CPPUNIT_ASSERT(13 == msg->_data.size());
  CPPUNIT_ASSERT(
    0 == memcmp(msg->_data.data(), buf, 13));
  log->debug("Server Receive2\n");
  MessageSPtr msg2 = conn_sock->receive();
  CPPUNIT_ASSERT(NULL != msg2);
  CPPUNIT_ASSERT(Socket::sliceSize + 1 == msg2->_data.size());
CPPUNIT_ASSERT(
    0 == memcmp((msg2->_data.data() + 1), refData, Socket::sliceSize));
  CPPUNIT_ASSERT(buf[0] == msg2->_data[0]);
  return NULL;
}

void* SocketTest::testSocketConnectTCPClient(void* sock) {
  sleep(3);
  log->debug("Sleep Done\n");
  Socket client_sock(ST_TCP);
  CPPUNIT_ASSERT(true == client_sock.connect("localhost", test_port));
  CPPUNIT_ASSERT(true == client_sock.isConnected());
  MessageSPtr msg(new Message(MT_None, MC_None));
  unsigned char buf[12];
  memcpy(buf, "Hello World!", 12);
  CPPUNIT_ASSERT(true == msg->addData(buf, 0, 13));
  log->debug("%s\n", msg->_data.data());
  CPPUNIT_ASSERT(
    0 == memcmp(msg->_data.data(), buf, 13));
  CPPUNIT_ASSERT(13 == msg->_data.size());
  CPPUNIT_ASSERT(true == client_sock.send(msg));
  CPPUNIT_ASSERT(true == msg->addData(refData, 1, Socket::sliceSize));
  CPPUNIT_ASSERT(
    0 == memcmp((msg->_data.data() + 1), refData, Socket::sliceSize));
  CPPUNIT_ASSERT(buf[0] == msg->_data[0]);
  CPPUNIT_ASSERT(Socket::sliceSize + 1 == msg->_data.size());
  CPPUNIT_ASSERT(true == client_sock.send(msg));
  return NULL;
}

CppUnit::Test* SocketTest::suite() {
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("SocketTest");
  addUnitTest(suite, SocketTest, testSocketConnectTCP);
  return suite;
}

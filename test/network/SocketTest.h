#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

namespace Doopey {
  class Socket;
  class SocketTest : public CppUnit::TestFixture {

    public:
      void setUp();
      void tearDown();
      void testSocketConnectTCP();
      static void* testSocketConnectTCPServer(void* sock);
      static void* testSocketConnectTCPClient(void* sock);

      static CppUnit::Test* suite();
      pthread_t server;
      pthread_t client;
      Socket* server_sock;
      static const int test_port = 10061;
  };
}

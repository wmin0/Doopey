#include "SocketTest.h"

#include <cppunit/ui/text/TestRunner.h>

using namespace Doopey;

int main(int argc, char** argv) {
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(SocketTest::suite());
  runner.run();
  return 0;
}

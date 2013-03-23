#include "LoggerTest.h"
#include <cppunit/ui/text/TestRunner.h>

int main() {
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(LoggerTest::suite());
  runner.run();
  return 0;
}

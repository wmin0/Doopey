#include "ConfigLoaderTest.h"
#include <cppunit/ui/text/TestRunner.h>

int main() {
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(ConfigLoaderTest::suite());
  runner.run();
  return 0;
}

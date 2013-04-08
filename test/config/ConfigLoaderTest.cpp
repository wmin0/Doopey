#include "ConfigLoaderTest.h"

#include "common/DoopeyTest.h"
#include "config/ConfigLoader.h"

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

using namespace Doopey;

void ConfigLoaderTest::testFunc() {
  //CPPUNIT_ASSERT(1 == 1);
  const char *path="TestConfig.txt";
  ConfigLoader::loadConfig(path);
}

CppUnit::Test* ConfigLoaderTest::suite() {
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("ConfigLoaderTest");
  addUnitTest(suite, ConfigLoaderTest, testFunc);
  return suite;
}

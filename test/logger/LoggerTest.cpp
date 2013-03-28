#include "LoggerTest.h"

#include "common/DoopeyTest.h"
#include "logger/Logger.h"

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

using namespace Doopey;

void LoggerTest::testLog() {
  //CPPUNIT_ASSERT(1 == 1);
  Logger logger(Debug);
  logger.debug("123");
  Logger logger2(Error);
  logger2.debug("123");
}

CppUnit::Test* LoggerTest::suite() {
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("LoggerTest");
  addUnitTest(suite, LoggerTest, testLog);
  return suite;
}

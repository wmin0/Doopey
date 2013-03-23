#include "LoggerTest.h"

#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestAssert.h>

#include "logger/Logger.h"

using namespace Doopey;

void LoggerTest::testLog() {
  //CPPUNIT_ASSERT(1 == 1);
  Logger logger(Debug);
  logger.debug("123");
  Logger logger2(Error);
  logger2.debug("123");
}

CppUnit::Test* LoggerTest::suite() {
  CppUnit::TestSuite* suiteOfTests = new CppUnit::TestSuite("LoggerTest");
  suiteOfTests->addTest(new CppUnit::TestCaller<LoggerTest>(
                        "testLog",
                        &LoggerTest::testLog));
  return suiteOfTests;
}

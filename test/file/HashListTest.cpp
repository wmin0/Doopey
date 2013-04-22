#include "HashListTest.h"

#include "common/Doopey.h"
#include "common/DoopeyTest.h"

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

using namespace Doopey;

void HashListTest::setUp()
{
  map = new HashList<string, string>(1);
  log.reset(new Logger(LL_Debug));
}

void HashListTest::tearDown()
{
  free(map);
}

void HashListTest::testHashList()
{
  log->debug("Adding test1\n");
  CPPUNIT_ASSERT(map->add("test1", "test1"));
  log->debug("Aadding test2\n");
  CPPUNIT_ASSERT(map->add("test2", "test2"));
  log->debug("Search for test2\n");
  CPPUNIT_ASSERT("test2" == map->getValue("test2"));
  log->debug("Search for test1\n");
  CPPUNIT_ASSERT("test1" == map->getValue("test1"));
  log->debug("Remove test2\n");
  CPPUNIT_ASSERT(map->remove("test2"));
  CPPUNIT_ASSERT("" == map->getValue("test2"));
  log->debug("Remove test1\n");
  CPPUNIT_ASSERT(map->remove("test1"));
}

CppUnit::Test* HashListTest::suite(){
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("HashListTest");
  addUnitTest(suite, HashListTest, testHashList);
  return suite;
}

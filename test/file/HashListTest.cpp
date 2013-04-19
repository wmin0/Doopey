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
}

void HashListTest::tearDown()
{
  free(map);
}

void HashListTest::testHashList()
{
  map->add("test1", "test1");
}

CppUnit::Test* HashListTest::suite(){
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("HashListTest");
  addUnitTest(suite, HashListTest, testHashList);
  return suite;
}

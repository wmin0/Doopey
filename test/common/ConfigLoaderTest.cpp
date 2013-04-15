#include "ConfigLoaderTest.h"

#include "common/DoopeyTest.h"
#include "common/ConfigLoader.h"
#include "common/SectionCollection.h"
#include "common/Config.h"

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

#include <iostream>
#include <memory>

using namespace Doopey;
using namespace std;

void ConfigLoaderTest::testFunc() {
  //CPPUNIT_ASSERT(1 == 1);
  const char *path="TestConfig.txt";
  shared_ptr<SectionCollection> sectionCollection(NULL);
  shared_ptr<Config> config(NULL);
  sectionCollection=ConfigLoader::loadConfig(path);

  config=sectionCollection->getConfig("config1");
  CPPUNIT_ASSERT(config->getValue("BlockDir1")=="/tmp1");
  CPPUNIT_ASSERT(config->getValue("BlockDir1-2")=="value1-2");

  config=sectionCollection->getConfig("config3");
  CPPUNIT_ASSERT(config->getValue("BlockDir3")=="/tmp3");

  config=sectionCollection->getConfig("config2");
  CPPUNIT_ASSERT(config->getValue("BlockDir2")=="/tmp2");
  CPPUNIT_ASSERT(config->getValue("BlockDir2-2")=="value2-2");
}

CppUnit::Test* ConfigLoaderTest::suite() {
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("ConfigLoaderTest");
  addUnitTest(suite, ConfigLoaderTest, testFunc);
  return suite;
}

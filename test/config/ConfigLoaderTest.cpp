#include "ConfigLoaderTest.h"

#include "common/DoopeyTest.h"
#include "config/ConfigLoader.h"
#include "config/SectionCollection.h"
#include "config/Config.h"

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

  shared_ptr<SectionCollection> sectionCollection;
  shared_ptr<Config> config;
  sectionCollection=ConfigLoader::loadConfig(path);

  config=sectionCollection->getConfig("config1");
  cout<<"config name is "<<config->name<<endl;
  cout<<"config property is "<<config->getValue("BlockDir1")<<endl;
  cout<<"config property is "<<config->getValue("BlockDir1-2")<<endl;

  config=sectionCollection->getConfig("config3");
  cout<<"config name is "<<config->name<<endl;
  cout<<"config property is "<<config->getValue("BlockDir3")<<endl;

  config=sectionCollection->getConfig("config2");
  cout<<"config name is "<<config->name<<endl;
  cout<<"config property is "<<config->getValue("BlockDir2")<<endl;
  cout<<"config property is "<<config->getValue("BlockDir2-2")<<endl;
}

CppUnit::Test* ConfigLoaderTest::suite() {
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("ConfigLoaderTest");
  addUnitTest(suite, ConfigLoaderTest, testFunc);
  return suite;
}

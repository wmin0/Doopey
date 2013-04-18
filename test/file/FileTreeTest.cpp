#include "FileTreeTest.h"

#include "common/Doopey.h"
#include "common/DoopeyTest.h"
#include "file/FileTree.h"

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

using namespace Doopey;

void FileTreeTest::testFileTree()
{
  
}

CppUnit::Test* FileTreeTest::suite(){
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("FileTreeTest");
  addUnitTest(suite, FileTreeTest, testFileTree);
  return suite;
}

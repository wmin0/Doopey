#include "FileTreeTest.h"

#include "common/Doopey.h"
#include "common/DoopeyTest.h"
#include "file/FileTree.h"

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

using namespace Doopey;

void FileTreeTest::setUp()
{
  file_table = new FileTree();
}

void FileTreeTest::tearDown()
{
  free(file_table);;
}

void FileTreeTest::testFileTree()
{
  CPPUNIT_ASSERT(file_table->addFile("/a", 1));
  CPPUNIT_ASSERT(file_table->addDir("/A"));
  CPPUNIT_ASSERT(file_table->addFile("/A/b", 2));
  CPPUNIT_ASSERT(1==file_table->getMetaID("/a"));
  CPPUNIT_ASSERT(2==file_table->getMetaID("/A/b"));
  CPPUNIT_ASSERT(file_table->removeFile("/A/b"));
  CPPUNIT_ASSERT(file_table->removeDir("/A"));
  CPPUNIT_ASSERT(file_table->removeFile("/a"));
}

CppUnit::Test* FileTreeTest::suite(){
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("FileTreeTest");
  addUnitTest(suite, FileTreeTest, testFileTree);
  return suite;
}

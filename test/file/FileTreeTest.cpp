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
  log.reset(new Logger(LL_Debug));
}

void FileTreeTest::tearDown()
{
  free(file_table);;
}

void FileTreeTest::showTree(int level, TreeNode* tn)
{
  TreeNode* show = tn;
  string s = "";
  for(int i=0; i<level;i++)
    s = s + " ";
  log->debug("%s%s\n", s.data(), show->_name.data());
  if(tn->_children != NULL)
  {
    show = tn->_children;
    while(show != NULL)
    {
      showTree(level+1, show);
      show = show->_next;
    }
  }
}

void FileTreeTest::testFileTree()
{
  string s = "/A/b/c";
  CPPUNIT_ASSERT("A" == file_table->getFirst(s)); 
  CPPUNIT_ASSERT("b/c" == s);
  CPPUNIT_ASSERT("c" == file_table->getFileName(s));
  log->debug("adding /A\n");
  CPPUNIT_ASSERT(file_table->addDir("/A"));
  log->debug("adding /A/b\n");
  CPPUNIT_ASSERT(file_table->addFile("/A/b", 2));
  log->debug("adding /A/c\n");
  CPPUNIT_ASSERT(file_table->addFile("/A/c", 3));
  showTree(0, file_table->_root);
  log->debug("try to get /A metaID\n");
  CPPUNIT_ASSERT(0==file_table->getMetaID("/A"));
  log->debug("try to get /A/b metaID\n");
  CPPUNIT_ASSERT(2==file_table->getMetaID("/A/b"));
  log->debug("try to remove /A/b\n");
  CPPUNIT_ASSERT(file_table->removeFile("/A/b"));
  showTree(0, file_table->_root);
  log->debug("try to remove /A\n");
  CPPUNIT_ASSERT(file_table->removeDir("/A"));
  showTree(0, file_table->_root);
}

CppUnit::Test* FileTreeTest::suite(){
  CppUnit::TestSuite* suite = new CppUnit::TestSuite("FileTreeTest");
  addUnitTest(suite, FileTreeTest, testFileTree);
  return suite;
}

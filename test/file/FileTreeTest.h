#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

namespace Doopey{
  class FileTree;
  class TreeNode;
  class FileTreeTest: public CppUnit::TestFixture {

    public:
      void setUp();
      void tearDown();
      void testFileTree();
      void showTree(int level, TreeNode* tn);

      static CppUnit::Test* suite();
      FileTree* file_table;
  };

}

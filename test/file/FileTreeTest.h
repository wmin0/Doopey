#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

namespace Doopey{

  class FileTreeTest: public CppUnit::TestFixture {

    public:
      void setUp() {}
      void tearDown() {}
      void testFileTree();

      static CppUnit::Test* suite();
  };

}

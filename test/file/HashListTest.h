#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

#include "file/FileTree.h"

using std::string;

namespace Doopey{

  template<typename KEY, typename VALUE>
  class HashList;

  class HashListTest: public CppUnit::TestFixture {
    public:
      void setUp();
      void tearDown();
      void testHashList();

      static CppUnit::Test* suite();

      HashList<string, string>* map;
  };
}

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

class ConfigLoaderTest : public CppUnit::TestFixture {

  public:
    void setUp() {}
    void tearDown() {}
    void testFunc();

    static CppUnit::Test* suite();
};

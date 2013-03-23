#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

class LoggerTest : public CppUnit::TestFixture {

  public:
    void setUp() {}
    void tearDown() {}
    void testLog();

    static CppUnit::Test* suite();
};

#ifndef _DOOPEY_TEST_H_
#define _DOOPEY_TEST_H_

namespace Doopey {

#define addUnitTest(SuitePtr, Class, Func) \
  SuitePtr->addTest(new CppUnit::TestCaller<Class>(#Func, &Class::Func))

};  // namespace Doopey

#endif

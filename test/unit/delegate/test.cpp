// Unit test code for Event::Connect

#include "test.hpp"

using namespace CppEvent;

Test::Test()
    : testing::Test()
{
}

Test::~Test()
{
  
}

/*
 *
 */
TEST_F(Test, delegate1)
{
  TestClassBase obj1;

  Delegate<void, int> d = Delegate<void, int>::from_method(&obj1, &TestClassBase::ConstMethod1);

  bool result = d.equal(&obj1, &TestClassBase::Method1);

  if (!result) {
    std::cout << "const not equal to non-const" << std::endl;
  }

  d(1);
  
  ASSERT_TRUE(!result);
}


TEST_F(Test, delegate2)
{
  TestClassBase obj1;
  Delegate<void, int> d1 = Delegate<void, int>::from_method(&obj1, &TestClassBase::ConstMethod1);
  Delegate<void, int> d2 = Delegate<void, int>::from_method(&obj1, &TestClassBase::Method1);

  if (d1 == d2) {
    std::cout << "2 delegates equal" << std::endl;
  }

  if (d1 != d2) {
    std::cout << " not equal" << std::endl;
  }

  if (d1 < d2) {
    std::cout << " less then " << std::endl;
  }

  if (d1 > d2) {
    std::cout << " great then " << std::endl;
  }
  
  d1(1);
  ASSERT_TRUE(true);  
}

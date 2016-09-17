// Unit test code for Event::Connect

#include "test.hpp"

using namespace sigcxx;

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

/*
 *
 */
TEST_F(Test, delegate3)
{
  TestClassBase obj1;

  Delegate<int, int> d = Delegate<int, int>::from_method(&obj1, &TestClassBase::MethodWithReturn);

  ASSERT_TRUE(d(1) == 1);
}

/*
 *
 */
TEST_F(Test, delegate4)
{
  TestClassBase obj1;
  
  Delegate<int, int> d;
  ASSERT_FALSE(d);
  
  d = Delegate<int, int>::from_method(&obj1, &TestClassBase::MethodWithReturn);
  
  ASSERT_TRUE(d(1) == 1);
}

/*
 *
 */
TEST_F(Test, delegate5)
{
  TestClassBase obj1;
  
  Delegate<void, int> d1(&obj1, &TestClassBase::ConstMethod1);
  Delegate<void, int> d2(&obj1, &TestClassBase::Method1);
  
  d2 = d1;
  
  d2(1);  // check the stdout print

  ASSERT_TRUE(d1 && d2);
}

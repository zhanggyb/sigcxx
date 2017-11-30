// Unit test code for Event::Connect

#include "test.hpp"

#include <typeinfo>

using namespace sigcxx;

Test::Test()
    : testing::Test() {
}

Test::~Test() {

}

TEST_F(Test, rtti) {
  TestClassBase obj;

  ASSERT_TRUE(typeid(&obj) == typeid(TestClassBase *));
}

/*
 *
 */
TEST_F(Test, delegate1) {
  TestClassBase obj1;

  DelegateT<void(int)> d3;

  DelegateT<void(int)> d = DelegateT<void(int)>::FromMethod(&obj1, &TestClassBase::ConstMethod1);

  bool result = d.Equal(&obj1, &TestClassBase::Method1);

  if (!result) {
    std::cout << "const not equal to non-const" << std::endl;
  }

  d(1);

  // And test delegate to private method:

  // uncomment this line should not work:
  //  DelegateT<void> dp = DelegateT<void>::FromMethod(&obj1, &TestClassBase::MethodInPrivate);

  // can only create delegate to private method in class methods, just what we want.
  obj1.TestPrivateMethod();

  ASSERT_TRUE(!result);
}

TEST_F(Test, delegate2) {
  TestClassBase obj1;
  DelegateT<void(int)> d1 = DelegateT<void(int)>::FromMethod(&obj1, &TestClassBase::ConstMethod1);
  DelegateT<void(int)> d2 = DelegateT<void(int)>::FromMethod(&obj1, &TestClassBase::Method1);

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
TEST_F(Test, delegate3) {
  TestClassBase obj1;

  DelegateT<int(int)> d = DelegateT<int(int)>::FromMethod(&obj1, &TestClassBase::MethodWithReturn);

  ASSERT_TRUE(d(1) == 1);
}

/*
 *
 */
TEST_F(Test, delegate4) {
  TestClassBase obj1;

  DelegateT<int(int)> d;
  ASSERT_FALSE(d);

  d = DelegateT<int(int)>::FromMethod(&obj1, &TestClassBase::MethodWithReturn);

  ASSERT_TRUE(d(1) == 1);
}

/*
 *
 */
TEST_F(Test, delegate5) {
  TestClassBase obj1;

  DelegateT<void(int)> d1(&obj1, &TestClassBase::ConstMethod1);
  DelegateT<void(int)> d2(&obj1, &TestClassBase::Method1);

  d2 = d1;

  d2(1);  // check the stdout print

  ASSERT_TRUE(d1 && d2);
}

TEST_F(Test, delegate_ref_1) {
  TestClassBase obj1;

  DelegateT<void(int)> d1(&obj1, &TestClassBase::Method1);
  DelegateT<void(int)> d2(&obj1, &TestClassBase::ConstMethod1);

  DelegateRefT<void(int)> r1(d1);
  DelegateRefT<void(int)> r2(d2);

  r1.Bind(&obj1, &TestClassBase::Method1);
  r2.Bind(&obj1, &TestClassBase::ConstMethod1);

  ASSERT_TRUE(r1 && r2);

  bool result1 = r1.IsBoundTo(&obj1, &TestClassBase::Method1);
  bool result2 = r2.IsBoundTo(&obj1, &TestClassBase::ConstMethod1);

  ASSERT_TRUE(result1 && result2);

  r1.Reset();
  r2.Reset();

  ASSERT_TRUE((!r1) && (!r2));
}

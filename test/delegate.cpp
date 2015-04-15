#include <iostream>
#include <cassert>

#include <cppevent/event.hpp>

using namespace CppEvent;

class TestClassBase
{
 public:

  TestClassBase ()
  {
  }

  virtual ~TestClassBase ()
  {
  }

  virtual void Method0 ()
  {
    std::cout << "Method0 in base class" << std::endl;
  }

  virtual void Method1 (int p0)
  {
    std::cout << "Method1 in base class, param0: " << p0 << std::endl;
  }

  virtual void ConstMethod1 (int p0)
  {
    std::cout << "ConstMethod1 in base class, param0: " << p0 << std::endl;
  }

  virtual void Method2 (int p0, int p1) const
  {
    std::cout << "const Method2 in base class, param0: " << p0 << " param1: "
              << p1 << std::endl;
  }

};

class TestClassDerived: public TestClassBase
{
 public:

  TestClassDerived ()
      : TestClassBase()
  {
  }

  virtual ~TestClassDerived ()
  {

  }

  virtual void Method0 () override
  {
    std::cout << "Method0 in sub class" << std::endl;
  }

  virtual void Method1 (int p0) override
  {
    std::cout << "Method1 in sub class, param0: " << p0 << std::endl;
  }

};

static void test_delegate01()
{
  TestClassBase obj1;

  Delegate<void, int> d = Delegate<void, int>::from_method(&obj1, &TestClassBase::Method1);

  // bool result = d.equal<TestClassBase, &TestClassBase::Method1>(&obj1);

  // assert(result);

  d(1);
}

static void test_delegate02()
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
}

int main (int argc, char* argv[])
{
  test_delegate01();

  test_delegate02();

  return 0;
}


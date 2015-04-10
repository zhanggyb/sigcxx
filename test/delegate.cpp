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

  Delegate<void, int> d = Delegate<void, int>::from_method<TestClassBase, &TestClassBase::Method1>(&obj1);

  bool result = d.equal<TestClassBase, &TestClassBase::Method1>(&obj1);

  assert(result);

  d(1);
}

static void test_event01()
{
  TestClassBase obj1;

  Event<int> event;
  event.connect<TestClassBase, &TestClassBase::Method1>(&obj1);
  event.Invoke(1);

  std::cout << "size of AbstractEvent: " << sizeof(event) << std::endl;
}

void test_event02()
{

}

int main (int argc, char* argv[])
{
  test_delegate01();
  test_event01();

  return 0;
}


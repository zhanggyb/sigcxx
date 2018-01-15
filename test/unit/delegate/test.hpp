// Unit test code for Event::connect

#pragma once

#include <gtest/gtest.h>

#include <sigcxx/sigcxx.hpp>

class Test : public testing::Test {
 public:
  Test();
  virtual ~Test();

 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

class TestClassBase {
 public:

  TestClassBase() {
  }

  virtual ~TestClassBase() {
  }

  void TestPrivateMethod() {
    // delegate to private method only within class method
    sigcxx::Delegate<void()> dp = sigcxx::Delegate<void()>::FromMethod(this, &TestClassBase::MethodInPrivate);
  }

  int MethodWithReturn(int n) {
    return n;
  }

  virtual void Method0() {
    std::cout << "Method0 in base class" << std::endl;
  }

  virtual void Method1(int p0) {
    std::cout << "Method1 in base class, param0: " << p0 << std::endl;
  }

  virtual void ConstMethod1(int p0) const {
    std::cout << "ConstMethod1 in base class, param0: " << p0 << std::endl;
  }

  virtual void Method2(int p0, int p1) const {
    std::cout << "const Method2 in base class, param0: " << p0 << " param1: "
              << p1 << std::endl;
  }

 private:
  void MethodInPrivate() {
    std::cout << "Cannot build delegate outside" << std::endl;
  }
};

class TestClassDerived : public TestClassBase {
 public:

  TestClassDerived()
      : TestClassBase() {
  }

  virtual ~TestClassDerived() {

  }

  virtual void Method0() override {
    std::cout << "Method0 in sub class" << std::endl;
  }

  virtual void Method1(int p0) override {
    std::cout << "Method1 in sub class, param0: " << p0 << std::endl;
  }

};

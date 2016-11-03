// Unit test code for Event::connect

#pragma once

#include <gtest/gtest.h>

#include <sigcxx/sigcxx.hpp>

class Test: public testing::Test
{
 public:
  Test ();
  virtual ~Test();

 protected:
  virtual void SetUp() {  }
  virtual void TearDown() {  }
};

class Source
{
 public:

  Source () { }

  ~Source () { }

  void DoTest1 (int n)
  {
    event1_.Emit(n);
  }

  void DoTest2 (int n1, int n2)
  {
    event2_.Emit(n1, n2);
  }

  inline sigcxx::Signal<int>& event1 ()
  {
    return event1_;
  }

  inline sigcxx::Signal<int, int>& event2 ()
  {
    return event2_;
  }

 private:

  sigcxx::Signal<int> event1_;
  sigcxx::Signal<int, int> event2_;
};

class AbstractConsumer: public sigcxx::Trackable
{
public:
  AbstractConsumer() {}
  virtual ~AbstractConsumer() {}

  virtual void OnVirtualTest (int, sigcxx::SLOT slot = nullptr) = 0;
};

class Consumer: public AbstractConsumer
{
 public:

  Consumer ()
 	 : test1_count_(0), test2_count_(0), virtualtest_count_(0)
 	 { }

  virtual ~Consumer () { }

  void OnTest1 (int n, sigcxx::SLOT /* slot */)
  {
	  test1_count_++;
	  std::cout << "Signal received in OnTest1, n " << n << ", " << test1_count_ << " times." << std::endl;
  }

  void OnTest2 (int n1, int n2, sigcxx::SLOT /* slot */)
  {
	  test2_count_++;
    std::cout << "Signal received in OnTest2, n1: " << n1 << " n2: " << n2 << ", " << test2_count_ << " times."
              << std::endl;
  }

  virtual void OnVirtualTest (int n, sigcxx::SLOT /* slot */) override
  {
    virtualtest_count_++;
    std::cout << "Virtual test in base class, param: " << n << ", " << virtualtest_count_ << " times."
              << std::endl;
  }

  inline size_t test1_count() const
  {
	  return test1_count_;
  }

  inline size_t test2_count() const
  {
	  return test2_count_;
  }

  inline size_t virtualtest_count() const
  {
	  return virtualtest_count_;
  }

protected:

  inline void set_virtualtest_count(size_t num)
  {
    virtualtest_count_ = num;
  }

 private:
  size_t test1_count_;
  size_t test2_count_;
  size_t virtualtest_count_;
};

class SubConsumer: public Consumer
{
 public:

  SubConsumer() {}

  virtual ~SubConsumer() {}

  virtual void OnVirtualTest (int n, sigcxx::SLOT /* slot */) override
  {
    set_virtualtest_count(virtualtest_count() + 1);
    std::cout << "Virtual test in sub class, param: " << n << ", " << virtualtest_count() << " times."
              << std::endl;
  }
};

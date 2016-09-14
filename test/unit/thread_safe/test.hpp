// Unit test code for Event::connect

#pragma once

#include <gtest/gtest.h>

#include <cppevent/event.hpp>

using cppevent::Sender;

class Test: public testing::Test
{
 public:
  Test ();
  virtual ~Test();

 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

class Source
{
 public:

  Source () { }

  ~Source () { }

  void DoTest1 (int n);

  void DoTest2 (int n1, int n2);

  inline cppevent::Event<int>& event1 ()
  {
    return event1_;
  }

  inline cppevent::Event<int, int>& event2 ()
  {
    return event2_;
  }

 private:

  cppevent::Event<int> event1_;
  cppevent::Event<int, int> event2_;
};

class Consumer: public cppevent::Trackable
{
 public:

  Consumer ()
      : test1_count_(0), test2_count_(0)
  { }

  virtual ~Consumer () { }

  void DisconnectAll ();

  void OnTest1 (const Sender* sender, int n)
  {
    test1_count_++;
    //std::cout << "Event received in OnTest1, n " << n << ", " << test1_count_ << " times." << std::endl;
  }

  void OnTest2 (const Sender* sender, int n1, int n2)
  {
    test2_count_++;
    //std::cout << "Event received in OnTest2, n1: " << n1 << " n2: " << n2 << ", " << test2_count_ << " times."
    //        << std::endl;
  }

  inline size_t test1_count() const
  {
    return test1_count_;
  }

  inline size_t test2_count() const
  {
    return test2_count_;
  }

 private:
  size_t test1_count_;
  size_t test2_count_;
};

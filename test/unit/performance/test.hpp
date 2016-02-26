// Unit test code for Event::connect

#pragma once

#include <gtest/gtest.h>

#include <cppevent/event.hpp>

class Test: public testing::Test
{
 public:
  Test ();
  virtual ~Test();

 protected:
  virtual void SetUp() {  }
  virtual void TearDown() {  }
};

class Consumer: public CppEvent::Trackable
{
 public:

  Consumer()
      : CppEvent::Trackable(),
        count_(0)
  {}

  virtual ~Consumer()
  {}

  void onCallback ()
  {
    count_++;
  }

  inline size_t count () const
  {
    return count_;
  }
  
 private:

  size_t count_;
};

class Source
{
 public:

  Source ()
  {}

  ~Source ()
  {}

  void DoTest ()
  {
    foo_.Invoke();
  }
  
  CppEvent::EventRef<> foo ()
  {
    return foo_;
  }
  
 private:

  CppEvent::Event<> foo_;
};

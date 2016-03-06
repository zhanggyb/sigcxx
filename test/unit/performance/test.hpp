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

class Consumer: public CppEvent::Observer
{
 public:

  Consumer()
      : CppEvent::Observer(),
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

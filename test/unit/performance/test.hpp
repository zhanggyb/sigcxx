// Unit test code for Event::connect

#pragma once

#include <gtest/gtest.h>

#include <cppevent/event.hpp>

using CppEvent::Sender;

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

  void onCallback (const Sender* sender)
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

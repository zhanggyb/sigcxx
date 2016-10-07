// Unit test code for Event::connect

#pragma once

#include <gtest/gtest.h>

#include <sigcxx/sigcxx.hpp>

using sigcxx::Slot;

class Test: public testing::Test
{
 public:
  Test ();
  virtual ~Test();

 protected:
  virtual void SetUp() {  }
  virtual void TearDown() {  }
};

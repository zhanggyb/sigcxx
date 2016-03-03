// Unit test code for Event::Connect

#include "test.hpp"
#include <iostream>

#define TEST_CYCLE_NUM 10000000

using namespace std;

Test::Test()
    : testing::Test()
{
}

Test::~Test()
{
  
}

TEST_F(Test, qt_signal_slot)
{
  QtSource s;
  QtConsumer c;

  QObject::connect(&s, &QtSource::valueChanged, &c, &QtConsumer::setValue);

  for(int i = 0; i < TEST_CYCLE_NUM; i++) {
    s.DoTest1(i);	// this should call 1 times
  }
  ASSERT_TRUE(true);
}

/*
 *
 */
TEST_F(Test, cppevent)
{
  Source s;
  Consumer c;

  s.event1().connect(&c, &Consumer::OnTest1);

  for(int i = 0; i < TEST_CYCLE_NUM; i++) {
    s.DoTest1(i);	// this should call 1 times
  }
  ASSERT_TRUE(true);
}

// Unit test code for Event::Connect

#include "test.hpp"
#include <iostream>
#include <subject.hpp>
#include <observer.hpp>

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
    s.emit_signal(i);
  }
  ASSERT_TRUE(true);
}

/*
 *
 */
TEST_F(Test, cppevent)
{
  Subject s;
  Observer c;

  s.signal1().connect(&c, &Observer::OnTest1IntegerParam);

  for(int i = 0; i < TEST_CYCLE_NUM; i++) {
    s.emit_signal1(i);
  }
  ASSERT_TRUE(true);
}

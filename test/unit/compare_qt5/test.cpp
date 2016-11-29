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

TEST_F(Test, qt_emit)
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
TEST_F(Test, sigcxx_emit)
{
  Subject s;
  Observer c;

  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);

  for(int i = 0; i < TEST_CYCLE_NUM; i++) {
    s.emit_signal1(i);
  }
  ASSERT_TRUE(true);
}

TEST_F(Test, qt_connect)
{
  QtSource s;
  QtConsumer c;

  for(int i = 0; i < TEST_CYCLE_NUM; i++) {
    QObject::connect(&s, &QtSource::valueChanged, &c, &QtConsumer::setValue);
  }
  s.emit_signal(1);
  ASSERT_TRUE(true);
}

TEST_F(Test, sigcxx_connect)
{
  Subject s;
  Observer c;

  for(int i = 0; i < TEST_CYCLE_NUM; i++) {
    s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  }
  s.emit_signal1(1);
  ASSERT_TRUE(true);
}

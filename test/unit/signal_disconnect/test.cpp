// Unit test code for Event::connect

#include "test.hpp"
#include <iostream>

#include <subject.hpp>
#include <observer.hpp>

using namespace std;
using sigcxx::Slot;

Test::Test()
    : testing::Test() {
}

Test::~Test() {

}

/*
 *
 */
TEST_F(Test, disconnect_once1) {
  Subject s1;
  Subject s2;
  Subject s3;
  Observer o;

  s1.signal1().connect(&o, &Observer::OnTest1IntegerParam);
  s2.signal1().connect(&o, &Observer::OnTest1IntegerParam);
  s3.signal1().connect(&o, &Observer::OnTest1IntegerParam);

  s2.signal1().disconnect_once(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE((o.CountBindings() == 2) &&
      (s1.signal1().count_connections() == 1) &&
      (s2.signal1().count_connections() == 0) &&
      (s3.signal1().count_connections() == 1));
}

/*
 * Disconnect 1
 */
TEST_F(Test, disconnect_once2) {
  Subject s;
  Observer c;

  s.signal1().connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().connect(&c, &Observer::OnTest1IntegerParam);

  s.signal1().disconnect_once(&c, &Observer::OnTest1IntegerParam, 0);
  s.signal1().disconnect_once(&c, &Observer::OnTest1IntegerParam, -1);

  s.emit_signal1(1);

  ASSERT_TRUE(c.test1_count() == 1);
}

/*
 *
 */
TEST_F(Test, disconnect_all) {
  Subject s;
  Observer o;

  s.signal1().connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().connect(&o, &Observer::OnTest1IntegerParam);

  s.signal1().disconnect_all(&o, &Observer::OnTest1IntegerParam);

  s.emit_signal1(1);

  ASSERT_TRUE(o.test1_count() == 0 && s.signal1().count_connections() == 0 && o.CountBindings() == 0);
}

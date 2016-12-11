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
TEST_F(Test, Disconnect1) {
  Subject s1;
  Subject s2;
  Subject s3;
  Observer o;

  s1.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s2.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s3.signal1().Connect(&o, &Observer::OnTest1IntegerParam);

  int count1 = s2.signal1().Disconnect(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE((o.CountBindings() == 2) &&
      (s1.signal1().CountConnections() == 1) &&
      (s2.signal1().CountConnections() == 0) &&
      (s3.signal1().CountConnections() == 1) && (count1 == 1));
}

/*
 * Disconnect 2
 */
TEST_F(Test, Disconnect2) {
  Subject s;
  Observer c;

  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);

  int count1 = s.signal1().Disconnect(&c, &Observer::OnTest1IntegerParam, 0);
  int count2 = s.signal1().Disconnect(&c, &Observer::OnTest1IntegerParam, -1);

  s.emit_signal1(1);

  ASSERT_TRUE(c.test1_count() == 1 && count1 == 1 && count2 == 1);
}

/*
 * Disconnect 3
 */
TEST_F(Test, Disconnect3) {
  Subject s;
  Observer c;

  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);

  int count1 = s.signal1().Disconnect(&c, &Observer::OnTest1IntegerParam, -1, 2);

  ASSERT_TRUE(c.CountBindings() == 1 && count1 == 2);
}

/*
 * Disconnect 4
 */
TEST_F(Test, Disconnect4) {
  Subject s;
  Observer c;

  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);

  int count1 = s.signal1().Disconnect(&c, &Observer::OnTest1IntegerParam, -1, 4);

  ASSERT_TRUE(c.CountBindings() == 0 && count1 == 3);
}

/*
 * Disconnect 5
 */
TEST_F(Test, Disconnect5) {
  Subject s;
  Observer c;

  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);

  int count1 = s.signal1().Disconnect(&c, &Observer::OnTest1IntegerParam, -1, -1);

  ASSERT_TRUE(c.CountBindings() == 0 && count1 == 3);
}

/*
 * Disconnect 6
 */
TEST_F(Test, Disconnect6) {
  Subject s;
  Observer c;

  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);

  int count1 = s.signal1().Disconnect(&c, &Observer::OnTest1IntegerParam, 0, 2);

  ASSERT_TRUE(c.CountBindings() == 1 && count1 == 2);
}

/*
 * Disconnect 7
 */
TEST_F(Test, Disconnect7) {
  Subject s;
  Observer c;

  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);

  int count1 = s.signal1().Disconnect(&c, &Observer::OnTest1IntegerParam, 2, 2);

  ASSERT_TRUE(c.CountBindings() == 2 && count1 == 1);
}

/*
 * Disconnect 7
 */
TEST_F(Test, Disconnect8) {
  Subject s;
  Observer c;

  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);

  int count1 = s.signal1().Disconnect(&c, &Observer::OnTest1IntegerParam, 0, 4);

  ASSERT_TRUE(c.CountBindings() == 0 && count1 == 3);
}

/*
 * Disconnect 9
 */
TEST_F(Test, Disconnect9) {
  Subject s;
  Observer c;

  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);

  int count1 = s.signal1().Disconnect(&c, &Observer::OnTest1IntegerParam, 0, -1);

  ASSERT_TRUE(c.CountBindings() == 0 && count1 == 3);
}

/*
 * Disconnect 10
 */
TEST_F(Test, Disconnect10) {
  Subject s;
  Observer c;

  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&c, &Observer::OnTest1IntegerParam);

  int count1 = s.signal1().Disconnect(&c, &Observer::OnTest1IntegerParam, -2, 2);

  ASSERT_TRUE(c.CountBindings() == 1 && count1 == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_all) {
  Subject s;
  Observer o;

  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);

  s.signal1().DisconnectAll(&o, &Observer::OnTest1IntegerParam);

  s.emit_signal1(1);

  ASSERT_TRUE(o.test1_count() == 0 && s.signal1().CountConnections() == 0 && o.CountBindings() == 0);
}

/*
 *
 */
TEST_F(Test, disconnect_no_check_1) {
  Subject s;
  Observer o;

  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);

  s.signal1().Disconnect(-1, 2);

  ASSERT_TRUE(s.signal1().CountConnections() == 3 && o.CountBindings() == 3);
}

/*
 *
 */
TEST_F(Test, disconnect_no_check_2) {
  Subject s;
  Observer o;

  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);

  s.signal1().Disconnect(0, 2);

  ASSERT_TRUE(s.signal1().CountConnections() == 3 && o.CountBindings() == 3);
}

/*
 *
 */
TEST_F(Test, disconnect_no_check_3) {
  Subject s;
  Observer o;

  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);

  s.signal1().Disconnect(-1, 6);  // by giving a very big counts, this is the same as DisconnectAll()

  ASSERT_TRUE(s.signal1().CountConnections() == 0 && o.CountBindings() == 0);
}

/*
 *
 */
TEST_F(Test, disconnect_no_check_4) {
  Subject s;
  Observer o;

  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);

  s.signal1().Disconnect(-1, -2); // by giving a negative of last parameter, this is the same as DisconnectAll()

  ASSERT_TRUE(s.signal1().CountConnections() == 0 && o.CountBindings() == 0);
}

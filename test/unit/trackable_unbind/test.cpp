// Unit test code for Event::Connect

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
TEST_F(Test, unbind_all1) {
  Subject s1;
  Subject s2;
  Observer o;

  s1.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s2.signal0().Connect(&o, &Observer::OnTest0);

  o.unbind_all();

  ASSERT_TRUE((s1.signal1().CountConnections() == 0) &&
      (s2.signal0().CountConnections() == 0) &&
      (o.CountBindings() == 0));
}

/*
 *
 */
TEST_F(Test, unbind_all2) {
  Subject s1;
  Subject s2;
  Observer o;

  s1.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s2.signal0().Connect(&o, &Observer::OnTest0);

  o.unbind_all(&Observer::OnTest1IntegerParam);

  ASSERT_TRUE((s1.signal1().CountConnections() == 0) &&
      (s2.signal0().CountConnections() == 1) &&
      (o.CountBindings() == 1));
}

TEST_F(Test, unbind_all3) {
  Subject s1;
  Subject s2;
  Observer o;

  s1.signal0().Connect(&o, &Observer::OnTest0);
  s1.signal0().Connect(&o, &Observer::OnTest0);
  s1.signal0().Connect(&o, &Observer::OnTest0);
  s1.signal0().Connect(&o, &Observer::OnTest0);

  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);

  o.unbind_all(&Observer::OnTestUnbindAllAt5);

  ASSERT_TRUE((o.CountBindings() == 4) &&
      (s1.signal0().CountConnections() == 4) &&
      (s2.signal0().CountConnections() == 0));
}

TEST_F(Test, unbind_all4) {
  Subject s1;
  Subject s2;
  Observer o;

  s1.signal0().Connect(&o, &Observer::OnTest0);
  s1.signal0().Connect(&o, &Observer::OnTest0);
  s1.signal0().Connect(&o, &Observer::OnTest0);
  s1.signal0().Connect(&o, &Observer::OnTest0);

  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);

  o.unbind_all();

  ASSERT_TRUE((o.CountBindings() == 0) &&
      (s1.signal0().CountConnections() == 0) &&
      (s2.signal0().CountConnections() == 0));
}

TEST_F(Test, unbind_once)
{
  Subject s;
  Observer o;

  s.signal0().Connect(&o, &Observer::OnTestUnbindOnceAt5);

  s.emit_signal0();  // 1
  ASSERT_TRUE((s.signal0().CountConnections() == 1) && (o.CountBindings() == 1) && (o.test0_count() == 1));

  s.emit_signal0();  // 2
  s.emit_signal0();  // 3
  s.emit_signal0();  // 4
  s.emit_signal0();  // 5

  ASSERT_TRUE((s.signal0().CountConnections() == 0) && (o.CountBindings() == 0) && (o.test0_count() == 5));

  s.emit_signal0();  // no connection

  ASSERT_TRUE((s.signal0().CountConnections() == 0) && (o.CountBindings() == 0) && (o.test0_count() == 5));
}

TEST_F(Test, unbind_all_in_callback1) {
  Subject s;
  Observer o;

  s.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);

  for (int i = 0; i < 6; i++) {
    s.emit_signal0();
  }

  ASSERT_TRUE(o.CountBindings() == 0);
}

TEST_F(Test, unbind_all_in_callback2) {
  Subject s1;
  Subject s2;
  Observer o;

  s1.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s1.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s1.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s1.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);

  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllAt5);

  for (int i = 0; i < 6; i++) {
    s1.emit_signal0();
  }

  ASSERT_TRUE((o.CountBindings() == 0) &&
      (s1.signal0().CountConnections() == 0) &&
      (s2.signal0().CountConnections() == 0));
}

TEST_F(Test, unbind_all_method_in_callback) {
  Subject s1;
  Subject s2;
  Observer o;

  s1.signal0().Connect(&o, &Observer::OnTest0);
  s1.signal0().Connect(&o, &Observer::OnTest0);
  s1.signal0().Connect(&o, &Observer::OnTest0);
  s1.signal0().Connect(&o, &Observer::OnTest0);

  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllMethodAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllMethodAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllMethodAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllMethodAt5);
  s2.signal0().Connect(&o, &Observer::OnTestUnbindAllMethodAt5);

  for (int i = 0; i < 6; i++) {
    s2.emit_signal0();
  }

  ASSERT_TRUE((o.CountBindings() == 4) &&
      (s1.signal0().CountConnections() == 4) &&
      (s2.signal0().CountConnections() == 0));
}

// Unit test code for Event::Connect

#include "test.hpp"
#include <iostream>

#include <subject.hpp>
#include <observer.hpp>

using namespace std;
using cppevent::Sender;

Test::Test()
    : testing::Test() {
}

Test::~Test() {

}

/*
 *
 */
TEST_F(Test, count_connections1) {
  Subject s;
  Observer o;

  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE(s.event1().CountConnections() == 3);
}

/*
 *
 */
TEST_F(Test, is_connected1) {
  Subject s;
  Observer o;

  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE(s.event1().IsConnected(&o, &Observer::OnTest1IntegerParam));
}

/*
 *
 */
TEST_F(Test, is_connected2) {
  Subject s1;
  Subject s2;

  s1.event1().Connect(s2.event1());

  ASSERT_TRUE(s1.event1().IsConnected(s2.event1()));
}

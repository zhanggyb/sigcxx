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

/*
 *
 */
TEST_F(Test, is_connected3) {
  Subject s1;
  Subject s2;
  Subject s3;
  Subject s4;
  Subject s5;

  Observer o1;
  Observer o2;
  Observer o3;

  s1.event1().Connect(&o1, &Observer::OnTest1IntegerParam);
  s2.event1().Connect(&o1, &Observer::OnTest1IntegerParam);
  s3.event1().Connect(&o1, &Observer::OnTest1IntegerParam);
  s4.event1().Connect(&o1, &Observer::OnTest1IntegerParam);
  s5.event1().Connect(&o1, &Observer::OnTest1IntegerParam);

  s1.event1().Connect(&o2, &Observer::OnTest1IntegerParam);
  s2.event1().Connect(&o2, &Observer::OnTest1IntegerParam);
  s3.event1().Connect(&o2, &Observer::OnTest1IntegerParam);
  s4.event1().Connect(&o2, &Observer::OnTest1IntegerParam);
  s5.event1().Connect(&o2, &Observer::OnTest1IntegerParam);

  s1.event1().Connect(&o3, &Observer::OnTest1IntegerParam);
  s2.event1().Connect(&o3, &Observer::OnTest1IntegerParam);
  s3.event1().Connect(&o3, &Observer::OnTest1IntegerParam);
  s4.event1().Connect(&o3, &Observer::OnTest1IntegerParam);
  s5.event1().Connect(&o3, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE(s1.event1().IsConnected(&o1) &&
      s2.event1().IsConnected(&o2) &&
      s3.event1().IsConnected(&o3));
}

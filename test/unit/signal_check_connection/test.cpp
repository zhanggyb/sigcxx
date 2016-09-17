// Unit test code for Event::Connect

#include "test.hpp"
#include <iostream>

#include <subject.hpp>
#include <observer.hpp>

using namespace std;
using sigcxx::Sender;

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

  s.event1().connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().connect(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE(s.event1().count_connections() == 3);
}

/*
 *
 */
TEST_F(Test, is_connected1) {
  Subject s;
  Observer o;

  s.event1().connect(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE(s.event1().is_connected(&o, &Observer::OnTest1IntegerParam));
}

/*
 *
 */
TEST_F(Test, is_connected2) {
  Subject s1;
  sigcxx::Signal<int> s2;

  s1.event1().connect(s2);

  ASSERT_TRUE(s1.event1().is_connected(s2));
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

  s1.event1().connect(&o1, &Observer::OnTest1IntegerParam);
  s2.event1().connect(&o1, &Observer::OnTest1IntegerParam);
  s3.event1().connect(&o1, &Observer::OnTest1IntegerParam);
  s4.event1().connect(&o1, &Observer::OnTest1IntegerParam);
  s5.event1().connect(&o1, &Observer::OnTest1IntegerParam);

  s1.event1().connect(&o2, &Observer::OnTest1IntegerParam);
  s2.event1().connect(&o2, &Observer::OnTest1IntegerParam);
  s3.event1().connect(&o2, &Observer::OnTest1IntegerParam);
  s4.event1().connect(&o2, &Observer::OnTest1IntegerParam);
  s5.event1().connect(&o2, &Observer::OnTest1IntegerParam);

  s1.event1().connect(&o3, &Observer::OnTest1IntegerParam);
  s2.event1().connect(&o3, &Observer::OnTest1IntegerParam);
  s3.event1().connect(&o3, &Observer::OnTest1IntegerParam);
  s4.event1().connect(&o3, &Observer::OnTest1IntegerParam);
  s5.event1().connect(&o3, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE(s1.event1().is_connected(&o1) &&
      s2.event1().is_connected(&o2) &&
      s3.event1().is_connected(&o3));
}

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
TEST_F(Test, count_connections1) {
  Subject s;
  Observer o;

  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE(s.signal1().CountConnections() == 3);
}

/*
 *
 */
TEST_F(Test, IsConnectedTo1) {
  Subject s;
  Observer o;

  s.signal1().Connect(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE(s.signal1().IsConnectedTo(&o, &Observer::OnTest1IntegerParam));
}

/*
 *
 */
TEST_F(Test, IsConnectedTo2) {
  Subject s1;
  sigcxx::Signal<int> s2;

  s1.signal1().Connect(s2);

  ASSERT_TRUE(s1.signal1().IsConnectedTo(s2));
}

/*
 *
 */
TEST_F(Test, IsConnectedTo3) {
  Subject s1;
  Subject s2;
  Subject s3;
  Subject s4;
  Subject s5;

  Observer o1;
  Observer o2;
  Observer o3;

  s1.signal1().Connect(&o1, &Observer::OnTest1IntegerParam);
  s2.signal1().Connect(&o1, &Observer::OnTest1IntegerParam);
  s3.signal1().Connect(&o1, &Observer::OnTest1IntegerParam);
  s4.signal1().Connect(&o1, &Observer::OnTest1IntegerParam);
  s5.signal1().Connect(&o1, &Observer::OnTest1IntegerParam);

  s1.signal1().Connect(&o2, &Observer::OnTest1IntegerParam);
  s2.signal1().Connect(&o2, &Observer::OnTest1IntegerParam);
  s3.signal1().Connect(&o2, &Observer::OnTest1IntegerParam);
  s4.signal1().Connect(&o2, &Observer::OnTest1IntegerParam);
  s5.signal1().Connect(&o2, &Observer::OnTest1IntegerParam);

  s1.signal1().Connect(&o3, &Observer::OnTest1IntegerParam);
  s2.signal1().Connect(&o3, &Observer::OnTest1IntegerParam);
  s3.signal1().Connect(&o3, &Observer::OnTest1IntegerParam);
  s4.signal1().Connect(&o3, &Observer::OnTest1IntegerParam);
  s5.signal1().Connect(&o3, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE(s1.signal1().IsConnectedTo(&o1) &&
      s2.signal1().IsConnectedTo(&o2) &&
      s3.signal1().IsConnectedTo(&o3));
}

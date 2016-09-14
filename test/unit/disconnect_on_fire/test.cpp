// Unit test code for Event::Connect

#include "test.hpp"

#include <subject.hpp>
#include <observer.hpp>
#include <iostream>

using namespace std;
using cppevent::Sender;

Test::Test()
    : testing::Test()
{
}

Test::~Test()
{

}

class Source
{
 public:

  Source () { }
  ~Source () { }

  void DoTest (int n = 0)
  {
    event_.Fire(n);
  }

  inline cppevent::Event<int>& event ()
  {
    return event_;
  }

 private:

  cppevent::Event<int> event_;
};

class Consumer: public cppevent::Trackable
{
 public:

  Consumer ()
  { }

  virtual ~Consumer () { }

  void OnTestNothing (const Sender* sender, int n)
  {
    // do nothing...
  }

  void OnTestDisconnectFirst (const Sender* sender, int n)
  {
    UnbindOnce(sender);
    // sender->event().DisconnectOnce(this, &Consumer::OnTestDisconnectFirst, 0);
  }

  void OnTestDisconnectLast (const Sender* sender, int n)
  {
    UnbindOnce(sender);
    // sender->event().DisconnectOnce(this, &Consumer::OnTestDisconnectLast, -1);
  }

  void OnTestDisconnectAll (const Sender* sender, int n)
  {
    // RemoveAllInConnections(sender);
    // sender->event().DisconnectAll(this, &Consumer::OnTestDisconnectAll);
    UnbindAll(this, &Consumer::OnTestDisconnectAll);
  }

};

/*
 *
 */
TEST_F(Test, disconnect_first_on_fire1)
{
  Source s;
  Consumer c;

  s.event().Connect(&c, &Consumer::OnTestDisconnectFirst);
  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestNothing);

  s.DoTest();
  ASSERT_TRUE(c.CountBindings() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_first_on_fire2)
{
  Source s;
  Consumer c;

  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestDisconnectFirst);
  s.event().Connect(&c, &Consumer::OnTestNothing);

  s.DoTest();
  ASSERT_TRUE(c.CountBindings() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_last_on_fire1)
{
  Source s;
  Consumer c;

  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestDisconnectLast);

  s.DoTest();
  ASSERT_TRUE(c.CountBindings() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_last_on_fire2)
{
  Source s;
  Consumer c;

  s.event().Connect(&c, &Consumer::OnTestDisconnectLast);
  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestNothing);

  s.DoTest();
  ASSERT_TRUE(c.CountBindings() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_all_on_fire1)
{
  Source s;
  Consumer c;

  s.event().Connect(&c, &Consumer::OnTestDisconnectAll);
  s.event().Connect(&c, &Consumer::OnTestDisconnectAll);
  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestDisconnectAll);
  s.event().Connect(&c, &Consumer::OnTestDisconnectAll);

  s.DoTest();
  ASSERT_TRUE(c.CountBindings() == 1);
}

TEST_F(Test, unbind_once)
{
  Subject s;
  Observer o;

  s.event0().Connect(&o, &Observer::OnTestUnbindOnceAt5);

  s.fire_event0();  // 1
  ASSERT_TRUE((s.event0().CountConnections() == 1) && (o.CountBindings() == 1) && (o.test0_count() == 1));

  s.fire_event0();  // 2
  s.fire_event0();  // 3
  s.fire_event0();  // 4
  s.fire_event0();  // 5

  ASSERT_TRUE((s.event0().CountConnections() == 0) && (o.CountBindings() == 0) && (o.test0_count() == 5));

  s.fire_event0();  // no connection

  ASSERT_TRUE((s.event0().CountConnections() == 0) && (o.CountBindings() == 0) && (o.test0_count() == 5));
}

TEST_F(Test, unbind_all_in_callback1) {
  Subject s;
  Observer o;

  s.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);

  for (int i = 0; i < 6; i++) {
    s.fire_event0();
  }
  
  ASSERT_TRUE(o.CountBindings() == 0);
}

TEST_F(Test, unbind_all_in_callback2) {
  Subject s1;
  Subject s2;
  Observer o;

  s1.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s1.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s1.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s1.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);

  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  
  for (int i = 0; i < 6; i++) {
    s1.fire_event0();
  }
  
  ASSERT_TRUE((o.CountBindings() == 5) &&
              (s1.event0().CountConnections() == 0) &&
              (s2.event0().CountConnections() == 5));
}

TEST_F(Test, unbind_all1) {
  Subject s1;
  Subject s2;
  Observer o;

  s1.event0().Connect(&o, &Observer::OnTest0);
  s1.event0().Connect(&o, &Observer::OnTest0);
  s1.event0().Connect(&o, &Observer::OnTest0);
  s1.event0().Connect(&o, &Observer::OnTest0);

  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  
  o.UnbindAll(&o, &Observer::OnTestUnbindAllAt5);
  
  ASSERT_TRUE((o.CountBindings() == 4) &&
              (s1.event0().CountConnections() == 4) &&
              (s2.event0().CountConnections() == 0));
}

TEST_F(Test, unbind_all2) {
  Subject s1;
  Subject s2;
  Observer o;

  s1.event0().Connect(&o, &Observer::OnTest0);
  s1.event0().Connect(&o, &Observer::OnTest0);
  s1.event0().Connect(&o, &Observer::OnTest0);
  s1.event0().Connect(&o, &Observer::OnTest0);

  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllAt5);
  
  o.UnbindAll();
  
  ASSERT_TRUE((o.CountBindings() == 0) &&
              (s1.event0().CountConnections() == 0) &&
              (s2.event0().CountConnections() == 0));
}

TEST_F(Test, unbind_all_method_in_callback) {
  Subject s1;
  Subject s2;
  Observer o;

  s1.event0().Connect(&o, &Observer::OnTest0);
  s1.event0().Connect(&o, &Observer::OnTest0);
  s1.event0().Connect(&o, &Observer::OnTest0);
  s1.event0().Connect(&o, &Observer::OnTest0);

  s2.event0().Connect(&o, &Observer::OnTestUnbindAllMethodAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllMethodAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllMethodAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllMethodAt5);
  s2.event0().Connect(&o, &Observer::OnTestUnbindAllMethodAt5);

  for (int i = 0; i < 6; i++) {
    s2.fire_event0();
  }

  ASSERT_TRUE((o.CountBindings() == 4) &&
              (s1.event0().CountConnections() == 4) &&
              (s2.event0().CountConnections() == 0));
}


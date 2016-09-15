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
    // sender->event_base().DisconnectOnce(this, &Consumer::OnTestDisconnectFirst, 0);
  }

  void OnTestDisconnectLast (const Sender* sender, int n)
  {
    UnbindOnce(sender);
    // sender->event_base().DisconnectOnce(this, &Consumer::OnTestDisconnectLast, -1);
  }

  void OnTestDisconnectAll (const Sender* sender, int n)
  {
    // RemoveAllInConnections(sender);
    // sender->event_base().DisconnectAll(this, &Consumer::OnTestDisconnectAll);
    UnbindAll(&Consumer::OnTestDisconnectAll);
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

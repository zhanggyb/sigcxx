// Unit test code for Event::Connect

#include "test.hpp"
#include <iostream>

using namespace std;

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

  void DoTest ()
  {
    event_.Fire(this);
  }

  inline CppEvent::EventRef<Source*> event ()
  {
    return event_;
  }

 private:

  CppEvent::Event<Source*> event_;
};

class Consumer: public CppEvent::Observer
{
 public:

  Consumer ()
  { }

  virtual ~Consumer () { }

  void OnTestNothing (Source* sender)
  {
    // do nothing...
  }

  void OnTestDisconnectFirst (Source* sender)
  {
    sender->event().disconnect(this, &Consumer::OnTestDisconnectFirst, CppEvent::DisconnectFirst);
  }

  void OnTestDisconnectLast (Source* sender)
  {
    sender->event().disconnect(this, &Consumer::OnTestDisconnectLast, CppEvent::DisconnectLast);
  }

  void OnTestDisconnectAll (Source* sender)
  {
    sender->event().disconnect(this, &Consumer::OnTestDisconnectAll, CppEvent::DisconnectAll);
  }

};

/*
 *
 */
TEST_F(Test, disconnect_first_on_fire1)
{
  Source s;
  Consumer c;

  s.event().connect(&c, &Consumer::OnTestDisconnectFirst);
  s.event().connect(&c, &Consumer::OnTestNothing);
  s.event().connect(&c, &Consumer::OnTestNothing);

  s.DoTest();
  ASSERT_TRUE(c.CountInConnections() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_first_on_fire2)
{
  Source s;
  Consumer c;

  s.event().connect(&c, &Consumer::OnTestNothing);
  s.event().connect(&c, &Consumer::OnTestDisconnectFirst);
  s.event().connect(&c, &Consumer::OnTestNothing);

  s.DoTest();
  ASSERT_TRUE(c.CountInConnections() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_last_on_fire1)
{
  Source s;
  Consumer c;

  s.event().connect(&c, &Consumer::OnTestNothing);
  s.event().connect(&c, &Consumer::OnTestNothing);
  s.event().connect(&c, &Consumer::OnTestDisconnectLast);

  s.DoTest();
  ASSERT_TRUE(c.CountInConnections() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_last_on_fire2)
{
  Source s;
  Consumer c;

  s.event().connect(&c, &Consumer::OnTestDisconnectLast);
  s.event().connect(&c, &Consumer::OnTestNothing);
  s.event().connect(&c, &Consumer::OnTestNothing);

  s.DoTest();
  ASSERT_TRUE(c.CountInConnections() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_all_on_fire1)
{
  Source s;
  Consumer c;

  s.event().connect(&c, &Consumer::OnTestDisconnectAll);
  s.event().connect(&c, &Consumer::OnTestDisconnectAll);
  s.event().connect(&c, &Consumer::OnTestNothing);
  s.event().connect(&c, &Consumer::OnTestDisconnectAll);
  s.event().connect(&c, &Consumer::OnTestDisconnectAll);

  s.DoTest();
  ASSERT_TRUE(c.CountInConnections() == 1);
}

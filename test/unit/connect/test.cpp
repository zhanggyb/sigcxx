// Unit test code for Event::Connect

#include "test.hpp"
#include <iostream>

using namespace std;
using CppEvent::Sender;

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

  void DoTest1 (int n)
  {
    event1_.Fire(n);
  }

  void DoTest2 (int n1, int n2)
  {
    event2_.Fire(n1, n2);
  }

  inline CppEvent::Event<int>& event1 ()
  {
    return event1_;
  }

  inline CppEvent::Event<int, int>& event2 ()
  {
    return event2_;
  }

 private:

  CppEvent::Event<int> event1_;
  CppEvent::Event<int, int> event2_;
};

class Consumer: public CppEvent::Trackable
{
 public:

  Consumer ()
      : test1_count_(0), test2_count_(0)
  { }

  virtual ~Consumer () { }

  void OnTest1 (const Sender* sender, int n)
  {
    test1_count_++;
    //std::cout << "Event received in OnTest1, n " << n << ", " << test1_count_ << " times." << std::endl;
  }

  void OnTest2 (const Sender* sender, int n1, int n2)
  {
    test2_count_++;
    //std::cout << "Event received in OnTest2, n1: " << n1 << " n2: " << n2 << ", " << test2_count_ << " times."
    // << std::endl;
  }

  void OnTestWithMeta (const Sender* sender, int n)
  {
    std::cout << "Event received in OnTestWithMeta, n: " << n << std::endl;
  }
  
  inline size_t test1_count() const
  {
    return test1_count_;
  }

  inline size_t test2_count() const
  {
    return test2_count_;
  }

 private:
  size_t test1_count_;
  size_t test2_count_;
};

class SelfDestroyConsumer: public CppEvent::Trackable
{
 public:

  static SelfDestroyConsumer* Create ();

  virtual ~SelfDestroyConsumer();

  void OnTest1 (const Sender* sender, int n);

 private:

  SelfDestroyConsumer();

};

class SelfConsumer: public CppEvent::Trackable
{
 public:

  SelfConsumer()
      : event_count_(0)
  {}

  virtual ~SelfConsumer()
  {}

  inline CppEvent::Event<>& event()
  {
    return event_;
  }

  void DoTest ()
  {
    event_.Fire();
  }

  void OnTest (const Sender* sender) {event_count_++;}

  inline size_t event_count() const
  {
    return event_count_;
  }

 private:

  CppEvent::Event<> event_;

  size_t event_count_;
};

SelfDestroyConsumer* SelfDestroyConsumer::Create()
{
  return new SelfDestroyConsumer();
}

SelfDestroyConsumer::SelfDestroyConsumer()
    : CppEvent::Trackable()
{}

SelfDestroyConsumer::~SelfDestroyConsumer()
{
  cout << "object destroyed" << endl;
}

void SelfDestroyConsumer::OnTest1(const Sender* sender, int n)
{
  DisconnectOnceFrom(sender);
  delete this;
}

/*
 *
 */
TEST_F(Test, connect_method_once)
{
  Source s;
  Consumer c;

  s.event1().Connect(&c, &Consumer::OnTest1);

  s.DoTest1(1);	// this should call 1 times
  ASSERT_TRUE(c.test1_count() == 1);
}

/*
 *
 */
TEST_F(Test, disconnect)
{
  Source s;
  Consumer c;

  s.event1().Connect(&c, &Consumer::OnTest1);
  s.event1().DisconnectAll(&c, &Consumer::OnTest1);

  s.DoTest1(1);	// nothing should be output in stdout
  ASSERT_TRUE(c.test1_count() == 0 && c.CountInConnections() == 0);
}

/*
 *
 */
TEST_F(Test, connect_method_4_times)
{
  Source s;
  Consumer c;

  s.event1().Connect(&c, &Consumer::OnTest1);
  s.event1().Connect(&c, &Consumer::OnTest1);
  s.event1().Connect(&c, &Consumer::OnTest1);
  s.event1().Connect(&c, &Consumer::OnTest1);

  s.DoTest1(1);	// this should call 4 times
  ASSERT_TRUE(c.test1_count() == 4 && s.event1().CountOutConnections() == 4 && c.CountInConnections() == 4);
}

/*
 *
 */
TEST_F(Test, disconnect_all)
{
  Source s;
  Consumer c;

  s.event1().Connect(&c, &Consumer::OnTest1);
  s.event1().Connect(&c, &Consumer::OnTest1);
  s.event1().Connect(&c, &Consumer::OnTest1);
  s.event1().Connect(&c, &Consumer::OnTest1);

  s.event1().DisconnectAll(&c, &Consumer::OnTest1);

  s.DoTest1(1);	// nothing should be output in stdout

  ASSERT_TRUE(c.test1_count() == 0 && s.event1().CountOutConnections() == 0 && c.CountInConnections() == 0);
}

/*
 * This testcase test the connectiong between events -- chain load
 */
TEST_F(Test, connect_event_once)
{
  Source s1;
  Source s2;
  Consumer c;

  s2.event1().Connect(&c, &Consumer::OnTest1);
  s1.event1().Connect(s2.event1());

  s1.DoTest1(1);  // cause chain event

  ASSERT_TRUE(c.test1_count() == 1);
}

/*
 * Disconnect 1
 */
TEST_F(Test, disconnect_once)
{
  Source s;
  Consumer c;

  s.event1().Connect(&c, &Consumer::OnTest1);
  s.event1().Connect(&c, &Consumer::OnTest1);
  s.event1().Connect(&c, &Consumer::OnTest1);

  s.event1().DisconnectOnce(&c, &Consumer::OnTest1, 0);
  s.event1().DisconnectOnce(&c, &Consumer::OnTest1, -1);

  s.DoTest1(1);

  ASSERT_TRUE(c.test1_count() == 1);
}

TEST_F(Test, delete_when_called)
{
  Source s;
  Consumer c1;
  Consumer c2;
  Consumer c3;
  SelfDestroyConsumer* obj = SelfDestroyConsumer::Create();

  s.event1().Connect(&c1, &Consumer::OnTest1);
  s.event1().Connect(obj, &SelfDestroyConsumer::OnTest1);
  s.event1().Connect(&c2, &Consumer::OnTest1);
  s.event1().Connect(&c3, &Consumer::OnTest1);

  s.DoTest1(1);  // check the stdout

  ASSERT_TRUE((c1.test1_count() == 1) &&
              (c2.test1_count() == 1) &&
              (c3.test1_count() == 1));
}

TEST_F(Test, selfconsumer)
{
  SelfConsumer c;

  c.event().Connect(&c, &SelfConsumer::OnTest);

  c.DoTest();
  c.DoTest();

  ASSERT_TRUE(c.event_count() == 2);
}

TEST_F(Test, event_chaining)
{
  Source s1;
  Source s2;
  Consumer c;

  s1.event1().Connect(s2.event1());
  s2.event1().Connect(&c, &Consumer::OnTest1);

  s1.DoTest1(1);

  ASSERT_TRUE(c.CountInConnections() == 1 && c.test1_count() == 1);
}

TEST_F(Test, delete_more_when_called)
{
  Source s;
  Consumer c1;
  Consumer c2;
  Consumer c3;
  SelfDestroyConsumer* obj1 = SelfDestroyConsumer::Create();
  SelfDestroyConsumer* obj2 = SelfDestroyConsumer::Create();
  SelfDestroyConsumer* obj3 = SelfDestroyConsumer::Create();

  s.event1().Connect(&c1, &Consumer::OnTest1);
  s.event1().Connect(obj1, &SelfDestroyConsumer::OnTest1);
  s.event1().Connect(&c2, &Consumer::OnTest1);
  s.event1().Connect(&c3, &Consumer::OnTest1);
  s.event1().Connect(obj2, &SelfDestroyConsumer::OnTest1);
  s.event1().Connect(obj3, &SelfDestroyConsumer::OnTest1);

  s.DoTest1(1);  // check the stdout

  ASSERT_TRUE((c1.test1_count() == 1) &&
              (c2.test1_count() == 1) &&
              (c3.test1_count() == 1));
}

TEST_F(Test, meta_connect)
{
  Source s;
  Consumer c;
  
  s.event1().Connect(&c, &Consumer::OnTestWithMeta);
  
  s.DoTest1(999);
  
  ASSERT_TRUE(true);
}

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

  void DoTest1 (int n)
  {
    event1_.Fire(n);
  }

  void DoTest2 (int n1, int n2)
  {
    event2_.Fire(n1, n2);
  }

  inline CppEvent::EventRef<int> event1 ()
  {
    return event1_;
  }

  inline CppEvent::EventRef<int, int> event2 ()
  {
    return event2_;
  }

 private:

  CppEvent::Event<int> event1_;
  CppEvent::Event<int, int> event2_;
};

class Consumer: public CppEvent::Observer
{
 public:

  Consumer ()
      : test1_count_(0), test2_count_(0)
  { }

  virtual ~Consumer () { }

  void DisconnectAll ()
  {
    RemoveAllInConnections();
  }

  void OnTest1 (int n)
  {
    test1_count_++;
    //std::cout << "Event received in OnTest1, n " << n << ", " << test1_count_ << " times." << std::endl;
  }

  void OnTest2 (int n1, int n2)
  {
    test2_count_++;
    //std::cout << "Event received in OnTest2, n1: " << n1 << " n2: " << n2 << ", " << test2_count_ << " times."
    // << std::endl;
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

class SelfDestroyConsumer: public CppEvent::Observer
{
 public:

  static SelfDestroyConsumer* Create ();

  virtual ~SelfDestroyConsumer();

  void OnTest1 (int n);

 private:

  SelfDestroyConsumer();

};

class SelfConsumer: public CppEvent::Observer
{
 public:

  SelfConsumer()
      : event_count_(0)
  {}

  virtual ~SelfConsumer()
  {}

  inline CppEvent::EventRef<> event()
  {
    return event_;
  }

  void DoTest ()
  {
    event_.Fire();
  }

  void OnTest () {event_count_++;}

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
    : CppEvent::Observer()
{}

SelfDestroyConsumer::~SelfDestroyConsumer()
{
  cout << "object destroyed" << endl;
}

void SelfDestroyConsumer::OnTest1(int n)
{
  delete this;
}

/*
 *
 */
TEST_F(Test, connect_method_once)
{
  Source s;
  Consumer c;

  s.event1().connect(&c, &Consumer::OnTest1);

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

  s.event1().connect(&c, &Consumer::OnTest1);
  s.event1().disconnect(&c, &Consumer::OnTest1);

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

  s.event1().connect(&c, &Consumer::OnTest1);
  s.event1().connect(&c, &Consumer::OnTest1);
  s.event1().connect(&c, &Consumer::OnTest1);
  s.event1().connect(&c, &Consumer::OnTest1);

  s.DoTest1(1);	// this should call 4 times
  ASSERT_TRUE(c.test1_count() == 4 && s.event1().count_out_connections() == 4 && c.CountInConnections() == 4);
}

/*
 *
 */
TEST_F(Test, disconnect_all)
{
  Source s;
  Consumer c;

  s.event1().connect(&c, &Consumer::OnTest1);
  s.event1().connect(&c, &Consumer::OnTest1);
  s.event1().connect(&c, &Consumer::OnTest1);
  s.event1().connect(&c, &Consumer::OnTest1);

  s.event1().disconnect(&c, &Consumer::OnTest1, CppEvent::DisconnectAll);
  
  s.DoTest1(1);	// nothing should be output in stdout
  
  ASSERT_TRUE(c.test1_count() == 0 && s.event1().count_out_connections() == 0 && c.CountInConnections() == 0);
}

/*
 * This testcase test the connectiong between events -- chain load
 */
TEST_F(Test, connect_event_once)
{
  Source s1;
  Source s2;
  Consumer c;

  s2.event1().connect(&c, &Consumer::OnTest1);
  s1.event1().connect(s2.event1());

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

  s.event1().connect(&c, &Consumer::OnTest1);
  s.event1().connect(&c, &Consumer::OnTest1);
  s.event1().connect(&c, &Consumer::OnTest1);

  s.event1().disconnect(&c, &Consumer::OnTest1, CppEvent::DisconnectFirst);
  s.event1().disconnect(&c, &Consumer::OnTest1, CppEvent::DisconnectLast);

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

  s.event1().connect(&c1, &Consumer::OnTest1);
  s.event1().connect(obj, &SelfDestroyConsumer::OnTest1);
  s.event1().connect(&c2, &Consumer::OnTest1);
  s.event1().connect(&c3, &Consumer::OnTest1);
  
  s.DoTest1(1);  // check the stdout

  ASSERT_TRUE((c1.test1_count() == 1) &&
              (c2.test1_count() == 1) &&
              (c3.test1_count() == 1));
}

TEST_F(Test, selfconsumer)
{
  SelfConsumer c;

  c.event().connect(&c, &SelfConsumer::OnTest);

  c.DoTest();
  c.DoTest();

  ASSERT_TRUE(c.event_count() == 2);
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
  
  s.event1().connect(&c1, &Consumer::OnTest1);
  s.event1().connect(obj1, &SelfDestroyConsumer::OnTest1);
  s.event1().connect(&c2, &Consumer::OnTest1);
  s.event1().connect(&c3, &Consumer::OnTest1);
  s.event1().connect(obj2, &SelfDestroyConsumer::OnTest1);
  s.event1().connect(obj3, &SelfDestroyConsumer::OnTest1);
  
  s.DoTest1(1);  // check the stdout

  ASSERT_TRUE((c1.test1_count() == 1) &&
              (c2.test1_count() == 1) &&
              (c3.test1_count() == 1));
}

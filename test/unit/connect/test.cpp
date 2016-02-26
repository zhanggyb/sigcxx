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
  ASSERT_TRUE(c.test1_count() == 0);
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
  ASSERT_TRUE(c.test1_count() == 4);
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

  s.event1().disconnect(&c, &Consumer::OnTest1);
  
  s.DoTest1(1);	// nothing should be output in stdout
  
  std::cout << sizeof(CppEvent::Event<int>) << std::endl;
  
  ASSERT_TRUE(c.test1_count() == 0);
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

  s.event1().disconnect1(&c, &Consumer::OnTest1);
  s.DoTest1(1);

  ASSERT_TRUE(c.test1_count() == 2);
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

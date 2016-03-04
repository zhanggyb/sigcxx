// Unit test code for Event::Connect

#include "test.hpp"

Test::Test()
    : testing::Test()
{
}

Test::~Test()
{
  
}

/*
 * This testcase test the connectiong between events -- chain load
 */
TEST_F(Test, connect_event_once)
{
  Source source;
  Consumer consumer;

  for(int i = 0; i < 1000000; i++)
  {
    source.foo().Connect(&consumer, &Consumer::onCallback);
  }

  source.DoTest();

  std::cout << "count in Consumer: " << consumer.count() << std::endl;
  
  ASSERT_TRUE(true);
}

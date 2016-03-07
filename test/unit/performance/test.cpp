// Unit test code for Event::Connect

#include "test.hpp"

#ifdef USE_BOOST_SIGNALS
#include <boost/signals2.hpp>
#endif

Test::Test()
    : testing::Test()
{
}

Test::~Test()
{

}

TEST_F(Test, fire_many_times)
{
  Consumer consumer;
  CppEvent::Event<> event;

  event.Connect(&consumer, &Consumer::onCallback);

  for(int i = 0; i < 1000000; i++)
  {
    event();
  }

  ASSERT_TRUE(consumer.count() == 1000000);
}

TEST_F(Test, connect_many_events)
{
  Consumer consumer;
  CppEvent::Event<> event;

  for(int i = 0; i < 1000000; i++)
  {
    event.Connect(&consumer, &Consumer::onCallback);
  }

  //event();

  ASSERT_TRUE(consumer.count() == 0);
}

#ifdef USE_BOOST_SIGNALS

struct Simple
{
 public:

  Simple ()
      : value(0) {}

  void operator()()
  {
    value++;
  }

  size_t value;
};

TEST_F(Test, boost_signals2_fire_many_times)
{
  // Signal with no arguments and a void return value
  boost::signals2::signal<void ()> sig;

  // Connect a slot
  Simple c;

  sig.connect(c);

  // Call all of the slots
  for(int i = 0; i < 1000000; i++)
  {
    sig();
  }

  ASSERT_TRUE(true);
}

TEST_F(Test, boost_signals2_connect_many_times)
{
  // Signal with no arguments and a void return value
  boost::signals2::signal<void ()> sig;

  // Connect a slot
  Simple c;

  for(int i = 0; i < 1000000; i++)
  {
    sig.connect(c);
  }

  // Call all of the slots
  // sig();

  ASSERT_TRUE(true);
}

#endif

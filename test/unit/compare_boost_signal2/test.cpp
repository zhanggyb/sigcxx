// Unit test code for Event::Connect

#include "test.hpp"

#include <observer.hpp>

#ifdef USE_BOOST_SIGNALS
#include <boost/signals2.hpp>
#endif

#define TEST_CYCLE_NUM 10000000

Test::Test()
    : testing::Test()
{
}

Test::~Test()
{

}

TEST_F(Test, fire_many_times)
{
  Observer consumer;
  sigcxx::Signal<> event;

  event.Connect(&consumer, &Observer::OnTest0);

  for(int i = 0; i < TEST_CYCLE_NUM; i++)
  {
    event();
  }

  ASSERT_TRUE(consumer.test0_count() == TEST_CYCLE_NUM);
}

TEST_F(Test, connect_many_events)
{
  Observer consumer;
  sigcxx::Signal<> event;

  for(int i = 0; i < TEST_CYCLE_NUM; i++)
  {
    event.Connect(&consumer, &Observer::OnTest0);
  }

  //signal_base();

  ASSERT_TRUE(consumer.test0_count() == 0);
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
  for(int i = 0; i < TEST_CYCLE_NUM; i++)
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

  for(int i = 0; i < TEST_CYCLE_NUM; i++)
  {
    sig.connect(c);
  }

  // Call all of the slots
  // sig();

  ASSERT_TRUE(true);
}

#endif

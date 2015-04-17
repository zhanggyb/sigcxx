/*
 */

#include <iostream>

#include "event-source.hpp"
#include "event-consumer.hpp"

static void event_to_trackable ()
{
  EventSource source;
  EventConsumer consumer;

  source.event1().Connect(&consumer, &EventConsumer::OnTest1);
  source.event2().Connect(&consumer, &EventConsumer::OnTest2);

  source.DoTest1(1);
  source.DoTest2(2, 2);
}

static void connect_and_disconnect ()
{
  EventSource source1;
  EventConsumer consumer;

  source1.event1().Connect(&consumer, &EventConsumer::OnTest1);
  source1.event2().Connect(&consumer, &EventConsumer::OnTest2);

  source1.event1().DisconnectOne(&consumer, &EventConsumer::OnTest1);

  source1.DoTest1(1);
  source1.DoTest2(2, 2);

  EventSource source2;
  source2.event2().Connect(source1.event2());
  source2.DoTest2(3, 3);

  source2.event2().DisconnectOne(source1.event2());
  std::cout << "disconnect events" << std::endl;
  source2.DoTest2(3, 3);
}

static void event_to_event ()
{
  EventSource source1;
  EventSource source2;
  EventConsumer consumer;

  source2.event1().Connect(&consumer, &EventConsumer::OnTest1);
  source2.event2().Connect(&consumer, &EventConsumer::OnTest2);

  source1.event1().Connect(source2.event1());
  source1.event2().Connect(source2.event2());

  source1.DoTest1(1);
  source2.DoTest2(2, 2);
}

static void disconnect_all_in_trackable ()
{
  EventSource source;
  EventConsumer consumer;

  source.event1().Connect(&consumer, &EventConsumer::OnTest1);
  source.event2().Connect(&consumer, &EventConsumer::OnTest2);

  consumer.DisconnectAll();

  source.DoTest1(1);
  source.DoTest2(2, 2);
}

static void self_consumer ()
{
  EventSelfConsumer consumer;

  consumer.event1().Connect(&consumer, &EventSelfConsumer::OnTest1);

  consumer.DoTest1(4);
}

static void delete_when_called ()
{
  EventSelfConsumer consumer;

  consumer.event0().Connect(&consumer, &EventSelfConsumer::OnTest0);

  consumer.DoTest0();
}

int main (int argc, char* argv[])
{
  std::cout << "------------------------------" << std::endl;

  event_to_trackable();

  std::cout << "------------------------------" << std::endl;

  connect_and_disconnect();

  std::cout << "------------------------------" << std::endl;

  event_to_event();

  std::cout << "------------------------------" << std::endl;

  disconnect_all_in_trackable();

  std::cout << "------------------------------" << std::endl;

  self_consumer();

  std::cout << "------------------------------" << std::endl;

  delete_when_called();

  return 0;
}

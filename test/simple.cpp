/*
 * simple.cpp
 *
 *  Created on: 2015年4月11日
 *      Author: zhanggy
 */

#include <iostream>

#include "event-source.hpp"
#include "event-consumer.hpp"

static void event_to_trackable ()
{
  EventSource source;
  EventConsumer consumer;

  source.event1().Connect<EventConsumer, &EventConsumer::OnTest1>(&consumer);
  source.event2().Connect<EventConsumer, &EventConsumer::OnTest2>(&consumer);

  source.DoTest1(1);
  source.DoTest2(2, 2);
}

static void event_to_event ()
{
  EventSource source1;
  EventSource source2;
  EventConsumer consumer;

  source2.event1().Connect<EventConsumer, &EventConsumer::OnTest1>(&consumer);
  source2.event2().Connect<EventConsumer, &EventConsumer::OnTest2>(&consumer);

  source1.event1().Connect(source2.event1());
  source1.event2().Connect(source2.event2());

  source1.DoTest1(1);
  source2.DoTest2(2, 2);
}

static void disconnect_all_in_trackable ()
{
  EventSource source;
  EventConsumer consumer;

  source.event1().Connect<EventConsumer, &EventConsumer::OnTest1>(&consumer);
  source.event2().Connect<EventConsumer, &EventConsumer::OnTest2>(&consumer);

  consumer.DisconnectAll();

  source.DoTest1(1);
  source.DoTest2(2, 2);
}

int main (int argc, char* argv[])
{
  std::cout << "------------------------------" << std::endl;

  event_to_trackable();

  std::cout << "------------------------------" << std::endl;

  event_to_event();

  std::cout << "------------------------------" << std::endl;

  disconnect_all_in_trackable();

  return 0;
}

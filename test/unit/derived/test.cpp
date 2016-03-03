// Unit test code for Event::Connect

#include "test.hpp"

Test::Test()
    : testing::Test()
      { }

Test::~Test()
  { }

/*
 *
 */
TEST_F(Test, connect_method_once)
{
  Consumer* obj1 = new Consumer;
  SubConsumer* obj2 = new SubConsumer;

  CppEvent::Event<int> event1;
  event1.Connect(obj1, &Consumer::OnVirtualTest);

  CppEvent::Event<int> event2;
  event2.Connect(obj2, &SubConsumer::OnVirtualTest); // this connect to the method in sub class

  CppEvent::Event<int> event3;
  event3.Connect(dynamic_cast<Consumer*>(obj2), &Consumer::OnVirtualTest); // this still connect to the method in sub class

  event1.Fire(1);
  event2.Fire(2);
  event3.Fire(3);

  bool result = (obj1->virtualtest_count() == 1) && (obj2->virtualtest_count() == 2);

  delete obj1;
  delete obj2;

  ASSERT_TRUE(result);
}

// static void event_to_trackable ()
// {
//   EventSource source;
//   EventConsumer consumer;
//
//   source.event1().connect(&consumer, static_cast<void (EventConsumer::*)(int)>(&EventConsumer::OnTest1));
//   source.event2().connect(&consumer, static_cast<void (EventConsumer::*)(int, int)>(&EventConsumer::OnTest2));
//
//   source.DoTest1(1);
//   source.DoTest2(2, 2);
// }
//
// static void connect_and_disconnect ()
// {
//   EventSource source1;
//   EventConsumer consumer;
//
//   source1.event1().connect(&consumer, static_cast<void (EventConsumer::*)(int)>(&EventConsumer::OnTest1));
//   source1.event2().connect(&consumer, static_cast<void (EventConsumer::*)(int, int)>(&EventConsumer::OnTest2));
//
//   source1.event1().disconnect1(&consumer, static_cast<void (EventConsumer::*)(int)>(&EventConsumer::OnTest1));
//
//   source1.DoTest1(1);
//   source1.DoTest2(2, 2);
//
//   EventSource source2;
//   source2.event2().connect(source1.event2());
//   source2.DoTest2(3, 3);
//
//   source2.event2().disconnect1(source1.event2());
//   std::cout << "disconnect events" << std::endl;
//   source2.DoTest2(3, 3);
// }
//
// static void event_to_event ()
// {
//   EventSource source1;
//   EventSource source2;
//   EventConsumer consumer;
//
//   source2.event1().connect(&consumer, static_cast<void (EventConsumer::*)(int)>(&EventConsumer::OnTest1));
//   source2.event2().connect(&consumer, static_cast<void (EventConsumer::*)(int, int)>(&EventConsumer::OnTest2));
//
//   source1.event1().connect(source2.event1());
//   source1.event2().connect(source2.event2());
//
//   source1.DoTest1(1);
//   source2.DoTest2(2, 2);
// }
//
// static void disconnect_all_in_trackable ()
// {
//   EventSource source;
//   EventConsumer consumer;
//
//   source.event1().connect(&consumer, static_cast<void (EventConsumer::*)(int)>(&EventConsumer::OnTest1));
//   source.event2().connect(&consumer, static_cast<void (EventConsumer::*)(int, int)>(&EventConsumer::OnTest2));
//
//   consumer.DisconnectAll();
//
//   source.DoTest1(1);
//   source.DoTest2(2, 2);
// }
//
// static void self_consumer ()
// {
//   EventSelfConsumer consumer;
//
//   consumer.event1().Connect(&consumer, &EventSelfConsumer::OnTest1);
//
//   consumer.DoTest1(4);
// }
//
// static void delete_when_called ()
// {
//   EventSelfConsumer consumer;
//
//   consumer.event0().Connect(&consumer, &EventSelfConsumer::OnTest0);
//
//   consumer.DoTest0();
// }

// int main (int argc, char* argv[])
// {
//   std::cout << std::endl;
//
//   std::cout << "---- Test Event --> Trackable: " << std::endl;
//
//   event_to_trackable();
//
//   std::cout << std::endl;
//
//   std::cout << "---- Test connect and disconnect: " << std::endl;
//
//   connect_and_disconnect();
//
//   std::cout << std::endl;
//
//   std::cout << "---- Test Event --> Event: " << std::endl;
//
//   event_to_event();
//
//   std::cout << std::endl;
//
//   std::cout << "---- Test disconnect all in trackable: " << std::endl;
//
//   disconnect_all_in_trackable();
//
//   std::cout << std::endl;
//
//   std::cout << "---- Test Event --> self: " << std::endl;
//
//   self_consumer();
//
//   std::cout << std::endl;
//
//   std::cout << "---- Test disconnect in the method invoked: " << std::endl;
//
//   delete_when_called();
//
//   std::cout << std::endl;
//
//   std::cout << "---- Test connect to virtual function: " << std::endl;
//
//   virtual_method_connection();
//
//   std::cout << std::endl;
//
//   return 0;
// }

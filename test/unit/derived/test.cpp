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

  cppevent::Event<int> event1;
  event1.Connect(obj1, &Consumer::OnVirtualTest);

  cppevent::Event<int> event2;
  event2.Connect(obj2, &SubConsumer::OnVirtualTest); // this connect to the method in sub class

  cppevent::Event<int> event3;
  event3.Connect(dynamic_cast<Consumer*>(obj2), &Consumer::OnVirtualTest); // this still connect to the method in sub class

  event1.Fire(1);
  event2.Fire(2);
  event3.Fire(3);

  bool result = (obj1->virtualtest_count() == 1) && (obj2->virtualtest_count() == 2);

  delete obj1;
  delete obj2;
  
  ASSERT_TRUE(result);
}

TEST_F(Test, connect_to_pure_virtual_function)
{
  cppevent::Event<int> event1;
  AbstractConsumer* obj1 = new Consumer;

  event1.Connect(obj1, &AbstractConsumer::OnVirtualTest);

  event1.Fire(1);

  int result = dynamic_cast<Consumer*>(obj1)->virtualtest_count();

  delete obj1;

  ASSERT_TRUE(result == 1);
}

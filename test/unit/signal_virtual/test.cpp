// Unit test code for Event::Connect

#include "test.hpp"

Test::Test()
    : testing::Test() {}

Test::~Test() {}

/*
 *
 */
TEST_F(Test, connect_method_once) {
  Consumer *obj1 = new Consumer;
  SubConsumer *obj2 = new SubConsumer;

  sigcxx::Signal<int> signal1;
  signal1.Connect(obj1, &Consumer::OnVirtualTest);

  sigcxx::Signal<int> signal2;
  signal2.Connect(obj2, &SubConsumer::OnVirtualTest); // this signal_connect to the method in sub class

  sigcxx::Signal<int> signal3;
  signal3.Connect(dynamic_cast<Consumer *>(obj2),
                 &Consumer::OnVirtualTest); // this still signal_connect to the method in sub class

  signal1.Emit(1);
  signal2.Emit(2);
  signal3.Emit(3);

  bool result = (obj1->virtualtest_count() == 1) && (obj2->virtualtest_count() == 2);

  delete obj1;
  delete obj2;

  ASSERT_TRUE(result);
}

TEST_F(Test, connect_to_pure_virtual_function) {
  sigcxx::Signal<int> signal1;
  AbstractConsumer *obj1 = new Consumer;

  signal1.Connect(obj1, &AbstractConsumer::OnVirtualTest);

  signal1.Emit(1);

  size_t result = dynamic_cast<Consumer *>(obj1)->virtualtest_count();

  delete obj1;

  ASSERT_TRUE(result == 1);
}

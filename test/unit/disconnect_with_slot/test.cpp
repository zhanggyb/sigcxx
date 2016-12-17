// Unit test code for Event::Connect

#include "test.hpp"

using namespace sigcxx;

Test::Test()
    : testing::Test() {
}

Test::~Test() {

}

class Consumer : public sigcxx::Trackable {
 public:

  enum Test {
    Undefined = 0,
    TestDisconnectMethod_End_1,
    TestDisconnectMethod_Start_1,
    TestDisconnectMethod_End_Count_5,
    TestDisconnectMethod_2_5,
    TestDisconnectMethod_Complex_1_2,
    TestDisconnectAllTest1
  };

  Consumer()
  : test(0), done(false) {}

  virtual ~Consumer() {}

  void OnTestDisconnect1(SLOT slot);

  void OnTestDisconnect2(SLOT slot);

  template<typename T, typename ... ParamTypes>
  void unbind_all(void (T::*method)(ParamTypes...)) {
    UnbindAll(method);
  };

  void unbind_all() {
    UnbindAll();
  }

  int test;

  bool done;

};

Signal<> sig1;
Consumer consumer;

void Consumer::OnTestDisconnect1(SLOT slot) {
  switch (test) {
    case TestDisconnectMethod_End_1: {
      if (!done) {
        sig1.Disconnect(&consumer, &Consumer::OnTestDisconnect1);
        done = true;
      }
      break;
    }
    case TestDisconnectMethod_Start_1: {
      if (!done) {
        sig1.Disconnect(&consumer, &Consumer::OnTestDisconnect1, 0);
        done = true;
      }
      break;
    }
    case TestDisconnectMethod_End_Count_5: {
      if (!done) {
        sig1.Disconnect(&consumer, &Consumer::OnTestDisconnect1, -1, 5);
        done = true;
      }
      break;
    }
    case TestDisconnectMethod_2_5: {
      if (!done) {
        sig1.Disconnect(&consumer, &Consumer::OnTestDisconnect1, 2, 5);
        done = true;
      }
      break;
    }
    case TestDisconnectMethod_Complex_1_2: {
      if (!done) {
        sig1.Disconnect(&consumer, &Consumer::OnTestDisconnect1, 1, 2);
        done = true;
      }
      break;
    }
    case TestDisconnectAllTest1: {
      if (!done) {
        sig1.DisconnectAll(&consumer, &Consumer::OnTestDisconnect1);
        done = true;
      }
      break;
    }
    default:
      break;
  }
}

void Consumer::OnTestDisconnect2(SLOT slot) {
  // nothing
}

/*
 *
 */
TEST_F(Test, disconnect_1) {

  // 5 connection
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);

  consumer.test = Consumer::Test::TestDisconnectMethod_End_1;

  sig1.Emit();

  ASSERT_TRUE(sig1.CountConnections() == 4 && consumer.CountBindings() == 4);
}

TEST_F(Test, disconnect_2) {

  sig1.DisconnectAll();
  consumer.unbind_all();
  consumer.done = false;

  // 5 connection
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);

  consumer.test = Consumer::Test::TestDisconnectMethod_Start_1;

  sig1.Emit();

  ASSERT_TRUE(sig1.CountConnections() == 4 && consumer.CountBindings() == 4);
}

TEST_F(Test, disconnect_3) {

  sig1.DisconnectAll();
  consumer.unbind_all();
  consumer.done = false;

  // 5 connection
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);

  consumer.test = Consumer::Test::TestDisconnectMethod_End_Count_5;

  sig1.Emit();

  ASSERT_TRUE(sig1.CountConnections() == 0 && consumer.CountBindings() == 0);
}

TEST_F(Test, disconnect_4) {

  sig1.DisconnectAll();
  consumer.unbind_all();
  consumer.done = false;

  // 5 connection
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);

  consumer.test = Consumer::Test::TestDisconnectMethod_2_5;

  sig1.Emit();

  ASSERT_TRUE(sig1.CountConnections() == 2 && consumer.CountBindings() == 2);
}

TEST_F(Test, disconnect_5) {

  sig1.DisconnectAll();
  consumer.unbind_all();
  consumer.done = false;

  // 5 connection
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect2);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect2);

  consumer.test = Consumer::Test::TestDisconnectMethod_Complex_1_2;

  sig1.Emit();

  ASSERT_TRUE(sig1.CountConnections() == 3 && consumer.CountBindings() == 3);
}

TEST_F(Test, disconnect_6) {

  sig1.DisconnectAll();
  consumer.unbind_all();
  consumer.done = false;

  // 5 connection
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect2);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect1);
  sig1.Connect(&consumer, &Consumer::OnTestDisconnect2);

  consumer.test = Consumer::Test::TestDisconnectAllTest1;

  sig1.Emit();

  ASSERT_TRUE(sig1.CountConnections() == 2 && consumer.CountBindings() == 2);
}

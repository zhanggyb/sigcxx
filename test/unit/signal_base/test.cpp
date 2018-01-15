// Unit test code for Event::Connect

#include "test.hpp"
#include <iostream>
#include <sigcxx/sigcxx.hpp>

using namespace std;
using namespace sigcxx;

Test::Test()
    : testing::Test() {
}

Test::~Test() {

}

class Source {
 public:

  Source() {}
  ~Source() {}

  void DoTest() {
    event_.Emit(this);
  }

  inline sigcxx::Signal<Source *> &event() {
    return event_;
  }

 private:

  sigcxx::Signal<Source *> event_;
};

class Consumer : public sigcxx::Trackable {
 public:

  Consumer() {}

  virtual ~Consumer() {}

  void OnTestNothing(Source *sender, __SLOT__) {
    // do nothing...
  }

};

TEST_F(Test, push_back1) {
  Consumer consumer;
  Signal<Source *> signal;

  signal.Connect(&consumer, &Consumer::OnTestNothing);
  signal.Connect(&consumer, &Consumer::OnTestNothing);
  signal.Connect(&consumer, &Consumer::OnTestNothing);

  ASSERT_TRUE(consumer.CountSignalBindings() == 3 && signal.CountConnections() == 3);
}

//TEST_F(Test, copy_observer)
//{
//  Source s;
//  Consumer c1;
//  Consumer c2;
// 
//  s.signal_base().Connect(&c1, &Consumer::OnTestNothing);
//
//  c2 = c1;
//  
//  ASSERT_TRUE(c1.CountConnections() == 1 && c2.CountConnections() == 0);
//}

// Unit test code for Event::Connect

#include "test.hpp"

#include <subject.hpp>
#include <observer.hpp>
#include <iostream>

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

  void DoTest(int n = 0) {
    event_.Emit(n);
  }

  inline sigcxx::Signal<int> &event() {
    return event_;
  }

 private:

  sigcxx::Signal<int> event_;
};

class Consumer : public sigcxx::Trackable {
 public:

  Consumer() {}

  virtual ~Consumer() {}

  void OnTestNothing(int /* n */, SLOT /* slot */) {
    // do nothing...
  }

  void OnTestDisconnectFirst(int n, SLOT slot) {
    Unbind(slot);
    // sender->signal_base().DisconnectOnce(this, &Consumer::OnTestDisconnectFirst, 0);
  }

  void OnTestDisconnectLast(int n, SLOT slot) {
    Unbind(slot);
    // sender->signal_base().DisconnectOnce(this, &Consumer::OnTestDisconnectLast, -1);
  }

  void OnTestDisconnectAll(int n, SLOT slot) {
    // RemoveAllInConnections(sender);
    // sender->signal_base().DisconnectAll(this, &Consumer::OnTestDisconnectAll);
    UnbindAll(&Consumer::OnTestDisconnectAll);
  }

};

/*
 *
 */
TEST_F(Test, disconnect_first_on_fire1) {
  Source s;
  Consumer c;

  s.event().Connect(&c, &Consumer::OnTestDisconnectFirst);
  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestNothing);

  s.DoTest();
  ASSERT_TRUE(c.CountBindings() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_first_on_fire2) {
  Source s;
  Consumer c;

  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestDisconnectFirst);
  s.event().Connect(&c, &Consumer::OnTestNothing);

  s.DoTest();
  ASSERT_TRUE(c.CountBindings() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_last_on_fire1) {
  Source s;
  Consumer c;

  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestDisconnectLast);

  s.DoTest();
  ASSERT_TRUE(c.CountBindings() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_last_on_fire2) {
  Source s;
  Consumer c;

  s.event().Connect(&c, &Consumer::OnTestDisconnectLast);
  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestNothing);

  s.DoTest();
  ASSERT_TRUE(c.CountBindings() == 2);
}

/*
 *
 */
TEST_F(Test, disconnect_all_on_fire1) {
  Source s;
  Consumer c;

  s.event().Connect(&c, &Consumer::OnTestDisconnectAll);
  s.event().Connect(&c, &Consumer::OnTestDisconnectAll);
  s.event().Connect(&c, &Consumer::OnTestNothing);
  s.event().Connect(&c, &Consumer::OnTestDisconnectAll);
  s.event().Connect(&c, &Consumer::OnTestDisconnectAll);

  s.DoTest();
  ASSERT_TRUE(c.CountBindings() == 1);
}

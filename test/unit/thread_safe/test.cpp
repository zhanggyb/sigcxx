// Unit test code for Event::Connect

#include "test.hpp"

using namespace std;

Test::Test()
    : testing::Test()
{
}

Test::~Test()
{
  
}

Source s;
Consumer c;

void thread1 () {
  int n = 100;

  for(int i = 0; i < n; i++) {
    cout << "thread1" << endl;
    s.event1().connect(&c, static_cast<void (Consumer::*)(int)>(&Consumer::OnTest1));
    s.DoTest1(1);
    s.event1().disconnect(&c, static_cast<void (Consumer::*)(int)>(&Consumer::OnTest1));
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void thread2 () {
  int n = 100;

  for(int i = 0; i < n; i++) {
    cout << "thread2" << endl;
    s.event1().connect(&c, static_cast<void (Consumer::*)(int)>(&Consumer::OnTest1));
    s.DoTest1(1);
    s.event1().disconnect(&c, static_cast<void (Consumer::*)(int)>(&Consumer::OnTest1));
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

/*
 *
 */
TEST_F(Test, connect_method_once)
{
  thread t1 (thread1);
  thread t2 (thread2);

  t1.join();
  t2.join();

  ASSERT_TRUE(true);
}


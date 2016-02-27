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

static std::mutex m;

void thread1 () {
  int n = 100;

  for(int i = 0; i < n; i++) {
    cout << "thread1" << endl;
    std::lock_guard<std::mutex> lock(m);
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
    std::lock_guard<std::mutex> lock(m);
    s.event1().connect(&c, static_cast<void (Consumer::*)(int)>(&Consumer::OnTest1));
    s.DoTest1(1);
    s.event1().disconnect(&c, static_cast<void (Consumer::*)(int)>(&Consumer::OnTest1));
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void thread3 () {
    Consumer* lc = 0;
    
    for(int i = 0; i < 1000; i++) {
        lc = new Consumer();
        s.event1().connect(lc, static_cast<void (Consumer::*)(int)>(&Consumer::OnTest1));
		    std::lock_guard<std::mutex> lock(m);
        s.DoTest1(3);
        delete lc;
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

TEST_F(Test, connect_consumer_in_threads)
{
    for(int i = 0; i < 100; i++) {
        thread t(thread3);
        t.detach();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50000));
}
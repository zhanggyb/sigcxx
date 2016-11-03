// Unit test code for Event::Connect

#include "test.hpp"
#include <thread>
#include <mutex>

using namespace std;
using sigcxx::SLOT;

static std::mutex m;

void Source::DoTest1(int n)
{
  lock_guard<mutex> lock(m);
  event1_.Emit(n);
}

void Source::DoTest2 (int n1, int n2)
{
  lock_guard<mutex> lock(m);
  event2_.Emit(n1, n2);
}

void Consumer::DisconnectAll()
{
  lock_guard<mutex> lock(m);
    UnbindAll();
}

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

  std::unique_lock<std::mutex> lock(m, std::defer_lock);

  for(int i = 0; i < 100; i++) {

    lock.lock();
    s.event1().Connect(&c, static_cast<void (Consumer::*)(int, SLOT)>(&Consumer::OnTest1));
    lock.unlock();

    s.DoTest1(1);

    lock.lock();
    s.event1().DisconnectAll(&c, static_cast<void (Consumer::*)(int, SLOT)>(&Consumer::OnTest1));
    lock.unlock();

    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void thread2 () {

  std::unique_lock<std::mutex> lock(m, std::defer_lock);

  for(int i = 0; i < 100; i++) {
    lock.lock();
    s.event1().Connect(&c, static_cast<void (Consumer::*)(int, SLOT)>(&Consumer::OnTest1));
    lock.unlock();

    s.DoTest1(1);

    lock.lock();
    s.event1().DisconnectAll(&c, static_cast<void (Consumer::*)(int, SLOT)>(&Consumer::OnTest1));
    lock.unlock();
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void thread3 () {
  Consumer* lc = 0;

  std::unique_lock<std::mutex> lock(m, std::defer_lock);

  for(int i = 0; i < 500; i++) {
    lc = new Consumer();

    lock.lock();
    s.event1().Connect(lc, static_cast<void (Consumer::*)(int, SLOT)>(&Consumer::OnTest1));
    lock.unlock();

    s.DoTest1(3);

    lock.lock();
    delete lc;
    lock.unlock();
  }
}

void thread4 () {
  Source* ls = 0;

  std::unique_lock<std::mutex> lock(m, std::defer_lock);

  for(int i = 0; i < 500; i++) {
    ls = new Source();

    lock.lock();
    ls->event1().Connect(&c, static_cast<void (Consumer::*)(int, SLOT)>(&Consumer::OnTest1));
    lock.unlock();

    ls->DoTest1(3);

    lock.lock();
    delete ls;
    lock.unlock();

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
  thread t[100];

  for(int i = 0; i < 100; i++) {
    t[i] = thread(thread3);
  }

  for(int i = 0; i < 100; i++) {
    t[i].join();
  }

  ASSERT_TRUE(true);
}

/*
 * Test complex sinario in which there're a large mount connection and disconnection
 *
 * It will take a long time to complete, and the result is always true.
 * The purpose is to make sure there's no segment fault occurs.
 */
TEST_F(Test, complex)
{
  thread ta[100];
  thread tb[100];
  thread tc[100];
  thread td[100];

  for(int i = 0; i < 100; i++) {
    ta[i] = thread(thread1);
  }
  for(int i = 0; i < 100; i++) {
    tb[i] = thread(thread2);
  }
  for(int i = 0; i < 100; i++) {
    tc[i] = thread(thread3);
  }
  for(int i = 0; i < 100; i++) {
    td[i] = thread(thread4);
  }

  for(int i = 0; i < 100; i++) {
    ta[i].join();
  }
  for(int i = 0; i < 100; i++) {
    tb[i].join();
  }
  for(int i = 0; i < 100; i++) {
    tc[i].join();
  }
  for(int i = 0; i < 100; i++) {
    td[i].join();
  }

  ASSERT_TRUE(true);
}

#pragma once

#include <cppevent/event.hpp>

using CppEvent::Trackable;
using CppEvent::Sender;

class Subject: public Trackable
{
 public:

  Subject();

  virtual ~Subject();

  inline void fire_event0()
  {
    event_0_.Fire();
  }

  inline void fire_event1(int n)
  {
    event_1_int_.Fire(n);
  }

  inline void fire_event2(int n1, int n2)
  {
    event_2_int_.Fire(n1, n2);
  }

  void OnTest1IntegerParam(const Sender* sender, int n);

  void OnTest2IntegerParams(const Sender* sender, int n1, int n2);

  inline CppEvent::Event<>& event0 ()
  {
    return event_0_;
  }

  inline CppEvent::Event<int>& event1 ()
  {
    return event_1_int_;
  }

  inline CppEvent::Event<int, int>& event2 ()
  {
    return event_2_int_;
  }

  inline size_t test1_count () const
  {
    return test1_count_;
  }

  inline size_t test2_count () const
  {
    return test2_count_;
  }

 private:

  CppEvent::Event<> event_0_;
  CppEvent::Event<int> event_1_int_;
  CppEvent::Event<int, int> event_2_int_;
  size_t test1_count_;
  size_t test2_count_;

};
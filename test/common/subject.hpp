#pragma once

#include <cppevent/event.hpp>

using cppevent::Trackable;
using cppevent::Sender;

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

  inline cppevent::EventRef<> event0 ()
  {
    return event_0_;
  }

  inline cppevent::EventRef<int> event1 ()
  {
    return event_1_int_;
  }

  inline cppevent::EventRef<int, int> event2 ()
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

  cppevent::Event<> event_0_;
  cppevent::Event<int> event_1_int_;
  cppevent::Event<int, int> event_2_int_;
  size_t test1_count_;
  size_t test2_count_;

};

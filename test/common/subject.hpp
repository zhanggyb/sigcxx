#pragma once

#include <sigcxx/sigcxx.hpp>

using sigcxx::Trackable;
using sigcxx::Sender;

class Subject: public Trackable
{
 public:

  Subject();

  virtual ~Subject();

  inline void fire_event0()
  {
    event_0_.Emit();
  }

  inline void fire_event1(int n)
  {
    event_1_int_.Emit(n);
  }

  inline void fire_event2(int n1, int n2)
  {
    event_2_int_.Emit(n1, n2);
  }

  void OnTest1IntegerParam(const Sender* sender, int n);

  void OnTest2IntegerParams(const Sender* sender, int n1, int n2);

  inline sigcxx::SignalRef<> event0 ()
  {
    return event_0_;
  }

  inline sigcxx::SignalRef<int> event1 ()
  {
    return event_1_int_;
  }

  inline sigcxx::SignalRef<int, int> event2 ()
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

  sigcxx::Signal<> event_0_;
  sigcxx::Signal<int> event_1_int_;
  sigcxx::Signal<int, int> event_2_int_;
  size_t test1_count_;
  size_t test2_count_;

};

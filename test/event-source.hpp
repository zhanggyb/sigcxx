/*
 */

#pragma once

#include <cppevent/event.hpp>

class EventSource
{
public:

  EventSource ()
  {
  }

  ~EventSource ()
  {
  }

  void DoTest1 (int n)
  {
    event1_.Invoke(n);
  }

  void DoTest2 (int n1, int n2)
  {
    event2_.Invoke(n1, n2);
  }

  CppEvent::Event<int>& event1 ()
  {
    return event1_;
  }

  CppEvent::Event<int, int>& event2 ()
  {
    return event2_;
  }

private:

  CppEvent::Event<int> event1_;
  CppEvent::Event<int, int> event2_;

};

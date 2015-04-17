/*
 */

#pragma once

#include <iostream>
#include <cppevent/event.hpp>

class EventConsumer: public CppEvent::Trackable
{

public:

  EventConsumer ()
  {

  }

  virtual ~EventConsumer ()
  {

  }

  void DisconnectAll ()
  {
    RemoveAllBindings();
  }

  void OnTest1 (int n)
  {
    std::cout << "Event received in OnTest1, n: " << n << std::endl;
  }

  void OnTest2 (int n1, int n2)
  {
    std::cout << "Event received in OnTest2, n1: " << n1 << " n2: " << n2
        << std::endl;
  }

};

class EventSelfConsumer: public CppEvent::Trackable
{

public:

  EventSelfConsumer ()
  {
    std::cout << "size of event: " << sizeof(event0_) << std::endl;
  }

  virtual ~EventSelfConsumer ()
  {

  }

  CppEvent::Event<>& event0 ()
  {
    return event0_;
  }

  CppEvent::Event<int>& event1 ()
  {
    return event1_;
  }

  void DoTest0 ()
  {
    event0_.Invoke();
  }

  void DoTest1 (int n)
  {
    event1_.Invoke(n);
  }

  void DisconnectAll ()
  {
    RemoveAllBindings();
  }

  void OnTest0 ()
  {
    std::cout << "Delete the connection" << std::endl;
    event0_.DisconnectOne(this, &EventSelfConsumer::OnTest0);
  }

  void OnTest1 (int n)
  {
    std::cout << "Event received in OnTest1, n: " << n << std::endl;
  }

private:

  CppEvent::Event<> event0_;

  CppEvent::Event<int> event1_;

};

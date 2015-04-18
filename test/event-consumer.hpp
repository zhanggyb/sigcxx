/*
 */

#pragma once

#include <iostream>
#include <cppevent/event.hpp>

class EventConsumerBase: public CppEvent::Trackable
{

public:

  EventConsumerBase ()
  {

  }

  virtual ~EventConsumerBase ()
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

  virtual void OnVirtualTest1 (int n)
  {
    std::cout << "Virtual test in base class, param: " << n << std::endl;
  }
};

class EventConsumer: public EventConsumerBase
{
 public:

  EventConsumer() {}

  virtual ~EventConsumer() {}

  virtual void OnVirtualTest1 (int n)
  {
    std::cout << "Virtual test in sub class, param: " << n << std::endl;
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
    event0_.Disconnect1(this, &EventSelfConsumer::OnTest0);
  }

  void OnTest1 (int n)
  {
    std::cout << "Event received in OnTest1, n: " << n << std::endl;
  }

private:

  CppEvent::Event<> event0_;

  CppEvent::Event<int> event1_;

};

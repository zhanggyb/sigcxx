/*
 * event-consumer.hpp
 *
 *  Created on: 2015年4月11日
 *      Author: zhanggy
 */

#pragma once

#include <iostream>
#include <cppevent/trackable.hpp>

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
    RemoveAllConnections();
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

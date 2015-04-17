# CppEvent

------

## Overview

A simple/header-only C++11 event/delegate framework based on fast C++ delegates.

This project is inspired by:
  * [Member Function Pointers and the Fastest Possible C++ Delegates](http://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible) by Don Clugston
  * [The Impossibly Fast C++ Delegates](http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates) by by Sergey Ryazanov
  * [Fast C++ Delegate: Boost.Function 'drop-in' replacement and multicast](http://www.codeproject.com/Articles/18389/Fast-C-Delegate-Boost-Function-drop-in-replacement) by JaeWook Choi
  * [CppEvents](http://code.google.com/p/cpp-events/) by Nickolas V. Pohilets

CppEvent currently lives on [GitHub](https://github.com/zhanggyb/CppEvent).

## Download and Install

> Note: this project currently does not support MSVC, will be fixed later

* Checkout this repository
* Copy *include/cppevent* into your C++ project
* configure the correct include directory

## Basic Usage

Here is a simple example:

First, include necessary header

```c++
#include <cppevent/event.hpp>
```

```c++
// A class inherit CppEvent::Trackable can receive events
class Consumer: public CppEvent::Trackable
{
public:

  // A callback method:
  void OnDoSum (int a, int b);

  // ... other code
};
```

```c++
// An event source
class Source
{
public:

  // other code

  CppEvent::EventRef<int, int>& sum_event ()
  {
    return sum_event_;
  }

  void DoSum ()
  {
    sum_event_.Invoke(1, 2);
  }

private:

  CppEvent::Event<int, int> sum_event_;  
};
```

```c++
void main (void)
{
    Consumer consum;
    Source source;

    // connect event to an trackable object
    source.sum_event().Connect(&consum, &Consumer::OnDoSum);

    source.DoSum();

    source.sum_event().Disconnect(&consum, &Consumer::OnDoSum);
}
```

## License

This project is licensed under MIT License, which can be found in the LICENSE
file. The license is also included at the top of each source file.

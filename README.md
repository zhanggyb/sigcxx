sigcxx
======

## Overview

[sigcxx](https://github.com/zhanggyb/sigcxx) is a lightweight, fast C++11
signal/slot (event/delegate) framework, inspired by:

- [Member Function Pointers and the Fastest Possible C++ Delegates](http://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible)
by Don Clugston
- [The Impossibly Fast C++ Delegates](http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates) by Sergey Ryazanov
- [Fast C++ Delegate: Boost.Function 'drop-in' replacement and multicast](http://www.codeproject.com/Articles/18389/Fast-C-Delegate-Boost-Function-drop-in-replacement) by JaeWook Choi
- [CppEvents](http://code.google.com/p/cpp-events/) by Nickolas V. Pohilets

Originally [signals and slots](http://doc.qt.io/qt-5/signalsandslots.html) is a
language construct introduced in Qt for communication between objects. In other
language or framework, the similar technique is called [events and
delegates](https://www.google.com/search?q=event+delegate&ie=utf-8&oe=utf-8). Signal
and slot makes it easy to implement the Subject/Observer pattern while avoiding
boilerplate code. The concept is that objects (typically GUI widgets) can send
signals containing event information which can be received by other objects
using special functions known as slots. This is similar to C/C++ callbacks
(function pointers), but signal/slot system ensures the type-correctness of
callback arguments.

This project is another C++ signal/slot implementation, it requires and takes
advantage of C++11 standard, provides a simple and fast way by minimal and
comprehensible code.

## Features

- Based on fast C++ delegates
- Powered by variadic template in C++11
- Multicast
- Slot can be virtual and pure virtual
- Signal chaining
- Automatic disconnecting
- etc.

## Installation

Just drag the header files in `include/` and source files in `src/` to your
project. Or use CMake to build and install a library:

```shell
$ mkdir build
$ cd build
$ cmake ../ -DCMAKE_INSTALL_PREFIX=/usr/local
$ make
$ sudo make install
```

This will finally install 2 header files into
`/usr/local/include/sigcxx`, and a `libsigcxx.a` into
`/usr/local/lib`.

## Usage

Let's use an example to show how to use `sigcxx`. Assume that you are trying to
develop a new GUI application and need a signal/slot framework for communication
between GUI objects. This is a typical scenario in which a signal/slot
(event/delegate) works as a basic function. For example, when a button is
clicked, or the value of a slider bar is changed, you want to emit a signal to
notify another object to react and do something (update the framebuffer, change
the text displayed, etc.). In general, a button may have more than one `clicked`
signal, there may be 'toggled', 'hovered' signals. Such situation can be
illustrated as below:

<div  align="center">
<img src="doc/graphics/example.png" width = "400" alt="Example" align=center />
</div>

### Include necessary header file

There's only one header file need to be included:

```c++
#include <sigcxx/sigcxx.hpp>
```

### Inherit from `sigcxx::Trackable`

Not any member function can be connected by a signal, the slot method must be
member function of a `sigcxx::Trackable` or its subclass.

```c++
class Widget: public sigcxx::Trackable
{
public:
  // ...
};
```

The slot method must have the first argument of `sigcxx::SLOT slot`, the
following is the arguments received from a corresponding signal. You can define
arbitray number of arguments.

```c++
class Observer: public Widget
{
public:
  // ...

  void onUpdate1 (sigcxx::SLOT slot);
  void onUpdate2 (sigcxx::SLOT slot, const Foo* param);
};
```

### Decleare and expose signals in Subject

It's highly recommended to use the template class `sigcxx::Signal<>` to declare
signals as a member variable, and expose the signal with
`sigcxx::SignalRef<>`. Both of them are variadic templates and can take arbitray
number of template arguments.

```c++
class Subject
{
public:

    // ...

    inline sigcxx::SignalRef<> notify1 () {
       return notify1_;
    }

    inline sigcxx::SignalRef<const Foo*> notify2 () {
       return notify2_;
    }

protected:
    virtual void mousePressEvent (MouseEvent* event)
    {
        // ...
        notify1_.Emit();
    }

    virtual void keyPressEvent (KeyEvent* event)
    {
        // ...
        notify2_.Emit(foo);
    }

private:
    sigcxx::Signal<> notify1_;
    sigcxx::Signal<const Foo*> notify2_;
};
```

### Connect signals when you want

Use the `connect` method of `sigcxx::SignalRef<>`:

```c++
Subject subject;
Observer observer1;
Observer observer2;

// ...

subject.notify1().connect(&observer1, &Observer::onUpdate1);
subject.notify2().connect(&observer2, &Observer::onUpdate2);
```

Now when any event in `subject` is emitted, it will call corresponding method in
Observer objects. Note that the template arguments in the signal must match the
arguments of a slot method, except the first `sigcxx::SLOT`, it's just a
`typedef` of `const sigcxx::Slot *`, reserved to work as a signature to avoid
any method to be a slot and has special usage in runtime.

A signal supports multicast, can be connected to a virtual (even pure virtual)
function, it can also be disconnected manually or automatically when observer
object is destroyed. For more information, please see the [Wiki
page](https://github.com/zhanggyb/libcppevent/wiki).

## Known Issue

This project currently does not support MSVC.(FIXME)

## License

This project is licensed under MIT License, which can be found in the
[LICENSE](https://github.com/zhanggyb/sigcxx/blob/master/LICENSE) file.

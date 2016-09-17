sigcxx
======

(almost done, updating wiki and documentation)

## Overview

[sigcxx](https://github.com/zhanggyb/sigcxx) is a lightweight fast C++11
signal/slot framework, inspired by:

- [Member Function Pointers and the Fastest Possible C++ Delegates](http://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible)
by Don Clugston
- [The Impossibly Fast C++ Delegates](http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates) by Sergey Ryazanov
- [Fast C++ Delegate: Boost.Function 'drop-in' replacement and multicast](http://www.codeproject.com/Articles/18389/Fast-C-Delegate-Boost-Function-drop-in-replacement) by JaeWook Choi
- [CppEvents](http://code.google.com/p/cpp-events/) by Nickolas V. Pohilets

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

This will finally install 2 header files into `/usr/local/include/sigcxx`, and a
`libsigcxx.a` into `/usr/local/lib`.

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

- Include necessary header files

```c++
#include <sigcxx/sigcxx.hpp>
```

- Subclass the Observer from `sigcxx::Trackable`

```c++

class Observer: public sigcxx::Trackable
{
public:
    // ...

    void onUpdate1 (const sigcxx::Sender* sender);

    void onUpdate2 (const sigcxx::Sender* sender, const Foo* param);

};

```

- Decleare and expose signals in Subject

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
        notify1_.Fire();
    }

    virtual void keyPressEvent (KeyEvent* event)
    {
        // ...
        notify2_.Fire(foo);
    }

private:

    sigcxx::Signal<> notify1_;
    sigcxx::Signal<const Foo*> notify2_;

};

```

- Connect signals when you want

```c++

Subject subject;
Observer observer1;
Observer observer2;

// ...

subject.notify1().connect(&observer1, &Observer::onUpdate1);
subject.notify2().connect(&observer2, &Observer::onUpdate2);

```

Now when any event in `subject` is fired, it will call corresponding method in
Observer objects. Just as widgets in GUI applications.

A signal supports multicast, can be connected to a virtual (even pure virtual)
function, it can also be disconnected manually or automatically when observer
object is destroyed. For more information, please see the [Wiki
page](https://github.com/zhanggyb/libcppevent/wiki).

## Known Issue

This project currently does not support MSVC.(FIXME)

## License

This project is licensed under MIT License, which can be found in the
[LICENSE](https://github.com/zhanggyb/sigcxx/blob/master/LICENSE) file.

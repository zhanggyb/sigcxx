sigcxx
======

## Overview

[sigcxx](https://github.com/zhanggyb/sigcxx) is a C++11 signal/slot implementation, inspired by:

- [Member Function Pointers and the Fastest Possible C++ Delegates](http://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible)
by Don Clugston
- [The Impossibly Fast C++ Delegates](http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates) by Sergey Ryazanov
- [Fast C++ Delegate: Boost.Function 'drop-in' replacement and multicast](http://www.codeproject.com/Articles/18389/Fast-C-Delegate-Boost-Function-drop-in-replacement) by JaeWook Choi
- [CppEvents](http://code.google.com/p/cpp-events/) by Nickolas V. Pohilets

## Features

- Based on fast C++ delegates
- Powered by C++11 variadic template
- Multicast
- Slot can be virtual and pure virtual
- Signal chaining
- Automatic disconnecting

## Usage

Just drag the header files in `include/` and source files in `src/` to your
project. Or use CMake to build and install a library:

```shell
$ mkdir build
$ cd build
$ cmake ../ -DCMAKE_INSTALL_PREFIX=/usr/local
$ make
$ sudo make install
```

This will finally install 2 header files into `/usr/local/include/sigcxx`, and
a `libsigcxx.a` into `/usr/local/lib`.

## Example

`sigcxx` provides a simple and fast way to communicate between objects. This
is the basic function in GUI frameworks such as Qt, Gtkmm etc. Here's a simple
example to show how to use `sigcxx` to connect notification events as
illustrated below.

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

    inline sigcxx::EventRef<> notify1 () {
       return notify1_;
    }

    inline sigcxx::EventRef<const Foo*> notify2 () {
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

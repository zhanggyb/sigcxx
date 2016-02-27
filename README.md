# CppEvent

------

## Overview

A simple C++11 event/delegate framework based on fast C++ delegates.

This project is inspired by:

* [Member Function Pointers and the Fastest Possible C++ Delegates](http://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible)
by Don Clugston
* [The Impossibly Fast C++ Delegates](http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates) by by Sergey Ryazanov
* [Fast C++ Delegate: Boost.Function 'drop-in' replacement and multicast](http://www.codeproject.com/Articles/18389/Fast-C-Delegate-Boost-Function-drop-in-replacement) by JaeWook Choi
* [CppEvents](http://code.google.com/p/cpp-events/) by Nickolas V. Pohilets

## Download and Install

> Note: this project currently does not support MSVC, will be fixed later

* Checkout this repository
* Copy *include/cppevent* and *src/\** into your C++ project

## Basic Usage

Here is a simple example, in this example, we delcare *clicked* event
in Button and connect it to a member function of Dialog.

```c++
#include <cppevent/event.hpp>

class Widget: public CppEvent::Observer;

class Button: public Widget
{
public:

    Button(Widget* parent);

    // Event connection interface
    CppEvent::EventRef<> clicked()
    {return clicked_;}

protected:

    virtual void mousePress (MouseParams const & params);

private:

    // Event implementation
    CppEvent::Event<> clicked_;
};

void Button::mousePress(MouseParams const & params)
{
    clicked.Emit();
}

class Dialog: public Widget
{
public:

    Dialog (Widget* parent);

    void OnShowText ();
};

Button* btn = new Button;
Dialog* dlg = new Dialog;
btn->clicked().connect(dlg, &Dialog::OnShowText);
```

See [Wiki](https://github.com/zhanggyb/libCppEvent/wiki) for more information.

## License

This project is licensed under MIT License, which can be found in the LICENSE
file. The license is also included at the top of each source file.

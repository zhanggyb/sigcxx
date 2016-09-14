// Unit test code for Event::connect

#pragma once

#include <gtest/gtest.h>

#include <cppevent/event.hpp>
#include <QtCore>

using CppEvent::Sender;

class Test: public testing::Test
{
 public:
  Test ();
  virtual ~Test();

 protected:
  virtual void SetUp() {  }
  virtual void TearDown() {  }
};

class QtSource: public QObject
{
  Q_OBJECT

public:
  QtSource() {}
  virtual ~QtSource() {}

  void DoTest1 (int n)
  {
    emit valueChanged(n);
  }

  signals:

  void valueChanged (int);
};

class QtConsumer: public QObject
{
  Q_OBJECT

public:
  QtConsumer()
    : QObject(), count_(0)
  {}

  virtual ~QtConsumer () {}

  public slots:

  void setValue (int n)
  {
    count_++;
  }

private:
  size_t count_;
};

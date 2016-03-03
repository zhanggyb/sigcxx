// Unit test code for Event::connect

#pragma once

#include <gtest/gtest.h>

#include <cppevent/event.hpp>
#include <QtCore>

class Test: public testing::Test
{
 public:
  Test ();
  virtual ~Test();

 protected:
  virtual void SetUp() {  }
  virtual void TearDown() {  }
};

class Source
{
 public:

  Source () { }

  ~Source () { }

  void DoTest1 (int n)
  {
    event1_.Emit(n);
  }

  inline CppEvent::EventRef<int> event1 ()
  {
    return event1_;
  }

 private:

  CppEvent::Event<int> event1_;
};

class Consumer: public CppEvent::Observer
{
 public:

  Consumer ()
 	 : test1_count_(0)
 	 { }

  virtual ~Consumer () { }

  void DisconnectAll ()
  {
    RemoveAllBindings();
  }

  void OnTest1 (int n)
  {
	  test1_count_++;
  }

  inline size_t test1_count() const
  {
	  return test1_count_;
  }

 private:
  size_t test1_count_;
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
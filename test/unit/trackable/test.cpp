// Unit test code for Event::Connect

#include "test.hpp"
#include <iostream>

using namespace std;

Test::Test()
    : testing::Test()
{
}

Test::~Test()
{

}

struct VBinding: public cppevent::details::Binding
{
public:
  VBinding(int value = 0)
  : cppevent::details::Binding(), v(value)
  {}

  int v;
};

class Trackable: public cppevent::Trackable
{
public:
  Trackable ()
  : cppevent::Trackable()
  { }

  virtual ~Trackable()
  {}

  void push_back (VBinding* binding)
  {
    PushBackBinding(binding);
  }

  void push_front (VBinding* binding)
  {
    PushFrontBinding(binding);
  }

  void insert (int index, VBinding* binding)
  {
    InsertBinding(index, binding);
  }

  void print () const
  {
    VBinding* v = 0;
    for (cppevent::details::Binding* p = first_binding(); p; p = p->next) {
      v = static_cast<VBinding*>(p);
      cout << v->v << endl;
    }
  }

};

class Source
{
 public:

  Source () { }
  ~Source () { }

  void DoTest (int n)
  {
    event_.Fire(n);
  }

  inline cppevent::Event<int>& event ()
  {
    return event_;
  }

 private:

  cppevent::Event<int> event_;
};

class Consumer: public cppevent::Trackable
{
 public:

  Consumer ()
  { }

  virtual ~Consumer () { }

  void OnTestNothing (const cppevent::Sender* sender, int)
  {
    // do nothing...
  }
  
};

TEST_F(Test, push_back1)
{
  Trackable c;

  c.push_back(new VBinding(0));
  c.push_back(new VBinding(1));
  c.push_back(new VBinding(2));

  c.print();

  ASSERT_TRUE(c.CountBindings() == 3);
}

TEST_F(Test, push_front1)
{
  Trackable c;

  c.push_front(new VBinding(0));
  c.push_front(new VBinding(1));
  c.push_front(new VBinding(2));

  c.print();

  ASSERT_TRUE(c.CountBindings() == 3);
}

TEST_F(Test, insert1)
{
  Trackable c;

  // same as push_front
  c.insert(0, new VBinding(0));
  c.insert(0, new VBinding(1));
  c.insert(0, new VBinding(2));

  c.print();

  ASSERT_TRUE(c.CountBindings() == 3);
}

TEST_F(Test, insert2)
{
  Trackable c;

  // same as push back
  c.insert(-1, new VBinding(0));
  c.insert(-1, new VBinding(1));
  c.insert(-1, new VBinding(2));

  c.print();

  ASSERT_TRUE(c.CountBindings() == 3);
}

TEST_F(Test, insert3)
{
  Trackable c;

  // same as push back
  c.insert(-1, new VBinding(0));
  c.insert(-1, new VBinding(1));
  c.insert(-1, new VBinding(2));

  c.insert(1, new VBinding(3));

  c.print();

  ASSERT_TRUE(c.CountBindings() == 4);
}

TEST_F(Test, insert4)
{
  Trackable c;

  // same as push back
  c.insert(-1, new VBinding(0));
  c.insert(-1, new VBinding(1));
  c.insert(-1, new VBinding(2));

  c.insert(-5, new VBinding(3));

  c.print();

  ASSERT_TRUE(c.CountBindings() == 4);
}

TEST_F(Test, remove_connections_from_event)
{
  Source s;
  Consumer c;
  
  s.event().Connect(&c, &Consumer::OnTestNothing);
  ASSERT_TRUE(s.event().CountConnections() == 1);
  
  s.event().DisconnectAll();

  ASSERT_TRUE(s.event().CountConnections() == 0);
}

//TEST_F(Test, copy_observer)
//{
// Source s;
// Consumer c1;
// Consumer c2;
//
// s.event().Connect(&c1, &Consumer::OnTestNothing);
//
// c2 = c1;
//
// ASSERT_TRUE(c1.CountConnectionsFrom() == 1 && c2.CountBindings() == 0);
//}

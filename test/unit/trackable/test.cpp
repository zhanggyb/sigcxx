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

struct VBinding: public CppEvent::details::Binding
{
public:
  VBinding(int value = 0)
  : CppEvent::details::Binding(), v(value)
  {}

  int v;
};

class Trackable: public CppEvent::Trackable
{
public:
  Trackable ()
  : CppEvent::Trackable()
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
    for (CppEvent::details::Binding* p = first_binding(); p; p = p->next) {
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

  void DoTest ()
  {
    event_.Fire(this);
  }

  inline CppEvent::Event<Source*>& event ()
  {
    return event_;
  }

 private:

  CppEvent::Event<Source*> event_;
};

class Consumer: public CppEvent::Trackable
{
 public:

  Consumer ()
  { }

  virtual ~Consumer () { }

  void OnTestNothing (const CppEvent::Meta* meta_ptr, Source* sender)
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

  ASSERT_TRUE(c.CountInConnections() == 3);
}

TEST_F(Test, push_front1)
{
  Trackable c;

  c.push_front(new VBinding(0));
  c.push_front(new VBinding(1));
  c.push_front(new VBinding(2));

  c.print();

  ASSERT_TRUE(c.CountInConnections() == 3);
}

TEST_F(Test, insert1)
{
  Trackable c;

  // same as push_front
  c.insert(0, new VBinding(0));
  c.insert(0, new VBinding(1));
  c.insert(0, new VBinding(2));

  c.print();

  ASSERT_TRUE(c.CountInConnections() == 3);
}

TEST_F(Test, insert2)
{
  Trackable c;

  // same as push back
  c.insert(-1, new VBinding(0));
  c.insert(-1, new VBinding(1));
  c.insert(-1, new VBinding(2));

  c.print();

  ASSERT_TRUE(c.CountInConnections() == 3);
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

  ASSERT_TRUE(c.CountInConnections() == 4);
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

  ASSERT_TRUE(c.CountInConnections() == 4);
}

TEST_F(Test, remove_connections_from_event)
{
  Source s;
  Consumer c;
  
  s.event().Connect(&c, &Consumer::OnTestNothing);
  
  std::cout << "hello" << std::endl;
  std::cout << s.event().CountOutConnections() << std::endl;
  
  s.event().RemoveAllOutConnections();

  std::cout << "show this line?" << std::endl;
  std::cout << s.event().CountOutConnections() << std::endl;
  
  ASSERT_TRUE(true);
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
// ASSERT_TRUE(c1.CountInConnections() == 1 && c2.CountInConnections() == 0);
//}

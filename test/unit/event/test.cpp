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

struct VToken: public CppEvent::details::InvokableToken<>
{
 public:
  VToken(int value = 0)
      : CppEvent::details::InvokableToken<>(), v(value)
  {}
  
  int v;
};

class Event: public CppEvent::Event<>
{
 public:
  Event ()
      : CppEvent::Event<>()
  { }
  
  virtual ~Event()
  {}
  
  void push_back (VToken* token)
  {
    PushBackToken(token);
  }
  
  void push_front (VToken* token)
  {
    PushFrontToken(token);
  }
  
  void insert (int index, VToken* token)
  {
    InsertToken(index, token);
  }
  
  void print () const
  {
    VToken* v = 0;
    for (CppEvent::details::Token* p = first_token(); p; p = p->next) {
      v = static_cast<VToken*>(p);
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

class Consumer: public CppEvent::Observer
{
 public:

  Consumer ()
  { }

  virtual ~Consumer () { }

  void OnTestNothing (Source* sender)
  {
    // do nothing...
  }

};

TEST_F(Test, push_back1)
{
  Event e;
  
  e.push_back(new VToken(0));
  e.push_back(new VToken(1));
  e.push_back(new VToken(2));
  
  e.print();
  
  ASSERT_TRUE(e.CountOutConnections() == 3);
}

TEST_F(Test, push_front1)
{
  Event c;
  
  c.push_front(new VToken(0));
  c.push_front(new VToken(1));
  c.push_front(new VToken(2));
  
  c.print();
  
  ASSERT_TRUE(c.CountOutConnections() == 3);
}

TEST_F(Test, insert1)
{
  Event c;
  
  // same as push_front
  c.insert(0, new VToken(0));
  c.insert(0, new VToken(1));
  c.insert(0, new VToken(2));
  
  c.print();
  
  ASSERT_TRUE(c.CountOutConnections() == 3);
}

TEST_F(Test, insert2)
{
  Event c;
  
  // same as push back
  c.insert(-1, new VToken(0));
  c.insert(-1, new VToken(1));
  c.insert(-1, new VToken(2));
  
  c.print();
  
  ASSERT_TRUE(c.CountOutConnections() == 3);
}

TEST_F(Test, insert3)
{
  Event c;
  
  // same as push back
  c.insert(-1, new VToken(0));
  c.insert(-1, new VToken(1));
  c.insert(-1, new VToken(2));
  
  c.insert(1, new VToken(3));
  
  c.print();
  
  ASSERT_TRUE(c.CountOutConnections() == 4);
}

TEST_F(Test, insert4)
{
  Event c;

  // same as push back
  c.insert(-1, new VToken(0));
  c.insert(-1, new VToken(1));
  c.insert(-1, new VToken(2));
  
  c.insert(-5, new VToken(3));
  
  c.print();
  
  ASSERT_TRUE(c.CountOutConnections() == 4);
}

//TEST_F(Test, copy_observer)
//{
//  Source s;
//  Consumer c1;
//  Consumer c2;
// 
//  s.event().Connect(&c1, &Consumer::OnTestNothing);
//
//  c2 = c1;
//  
//  ASSERT_TRUE(c1.CountOutConnections() == 1 && c2.CountOutConnections() == 0);
//}

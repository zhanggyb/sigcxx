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

TEST_F(Test, copy_observer)
{
  Source s;
  Consumer c1;
  Consumer c2;
 
  s.event().Connect(&c1, &Consumer::OnTestNothing);

  c2 = c1;
  
  ASSERT_TRUE(c1.CountInConnections() == 1 && c2.CountInConnections() == 0);
}

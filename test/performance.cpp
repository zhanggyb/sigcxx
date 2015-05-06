// Performance test

#include <iostream>
#include <cppevent/event.hpp>

class Consumer: public CppEvent::Trackable
{
 public:

  Consumer()
      : CppEvent::Trackable(),
        count_(0)
  {}

  virtual ~Consumer()
  {}

  void onCallback ()
  {
    count_++;
  }

  inline size_t count () const
  {
    return count_;
  }
  
 private:

  size_t count_;
};

class Source
{
 public:

  Source ()
  {}

  ~Source ()
  {}

  void DoTest ()
  {
    foo_.Invoke();
  }
  
  CppEvent::EventRef<> foo ()
  {
    return foo_;
  }
  
 private:

  CppEvent::Event<> foo_;
};

int main ()
{
  Source source;
  Consumer consumer;

  for(int i = 0; i < 1000000; i++)
  {
    source.foo().connect(&consumer, &Consumer::onCallback);
  }

  source.DoTest();

  std::cout << "count in Consumer: " << consumer.count() << std::endl;
  
  return 0;
}

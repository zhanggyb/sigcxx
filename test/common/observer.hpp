#pragma once

#include <sigcxx/sigcxx.hpp>

using sigcxx::Trackable;
using sigcxx::Sender;

class Observer: public Trackable
{
 public:

  Observer();

  virtual ~Observer();

  void OnTest0 (const Sender* sender);

  void OnTest1IntegerParam(const Sender* sender, int n);

  void OnTest2IntegerParams(const Sender* sender, int n1, int n2);

  void OnTestDestroy (const Sender* sender);

  void OnTestUnbindOnceAt5 (const Sender* sender);

  void OnTestUnbindAllAt5 (const Sender* sender);

  void OnTestUnbindAllMethodAt5 (const Sender* sender);

  inline size_t test0_count () const
  {
    return test0_count_;
  }

  inline size_t test1_count () const
  {
    return test1_count_;
  }

  inline size_t test2_count () const
  {
    return test2_count_;
  }

 private:

  size_t test0_count_;
  size_t test1_count_;
  size_t test2_count_;
};

#pragma once

#include <sigcxx/sigcxx.hpp>

class Observer: public sigcxx::Trackable
{
 public:

  Observer();

  virtual ~Observer();

  void OnTest0 (sigcxx::SLOT slot);

  void OnTest1IntegerParam(sigcxx::SLOT slot, int n);

  void OnTest2IntegerParams(sigcxx::SLOT slot, int n1, int n2);

  void OnTestDestroy (sigcxx::SLOT slot);

  void OnTestUnbindOnceAt5 (sigcxx::SLOT slot);

  void OnTestUnbindAllAt5 (sigcxx::SLOT slot);

  void OnTestUnbindAllMethodAt5 (sigcxx::SLOT slot);

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

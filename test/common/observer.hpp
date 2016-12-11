#pragma once

#include <sigcxx/sigcxx.hpp>

class Observer: public sigcxx::Trackable
{
 public:

  Observer();

  virtual ~Observer();

  void OnTest0 (sigcxx::SLOT slot);

  void OnTest1IntegerParam(int n, sigcxx::SLOT slot = nullptr);

  void OnTest2IntegerParams(int n1, int n2, sigcxx::SLOT slot = nullptr);

  void OnTestDestroy (sigcxx::SLOT slot = nullptr);

  void OnTestUnbindOnceAt5 (sigcxx::SLOT slot = nullptr);

  void OnTestUnbindAllAt5 (sigcxx::SLOT slot = nullptr);

  void OnTestUnbindAllMethodAt5 (sigcxx::SLOT slot = nullptr);

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

  template<typename T, typename ... ParamTypes>
  void unbind_all(void (T::*method)(ParamTypes...)) {
    UnbindAll(method);
  };

  void unbind_all() {
    UnbindAll();
  }

 private:

  size_t test0_count_;
  size_t test1_count_;
  size_t test2_count_;
};

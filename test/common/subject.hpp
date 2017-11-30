#pragma once

#include <sigcxx/sigcxx.hpp>

class Subject: public sigcxx::Trackable
{
 public:

  Subject();

  virtual ~Subject();

  inline void emit_signal0()
  {
    signal_0_.Emit();
  }

  inline void emit_signal1(int n)
  {
    signal_1_int_.Emit(n);
  }

  inline void emit_signal2(int n1, int n2)
  {
    signal_2_int_.Emit(n1, n2);
  }

  void OnTest1IntegerParam(int n, sigcxx::SLOT slot = nullptr);

  void OnTest2IntegerParams(int n1, int n2, sigcxx::SLOT slot = nullptr);

  inline sigcxx::SignalRefT<> signal0()
  {
    return signal_0_;
  }

  inline sigcxx::SignalRefT<int> signal1()
  {
    return signal_1_int_;
  }

  inline sigcxx::SignalRefT<int, int> signal2()
  {
    return signal_2_int_;
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

  sigcxx::SignalT<> signal_0_;
  sigcxx::SignalT<int> signal_1_int_;
  sigcxx::SignalT<int, int> signal_2_int_;
  size_t test1_count_;
  size_t test2_count_;

};

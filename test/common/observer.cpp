#include "observer.hpp"

#include <iostream>

using sigcxx::Trackable;
using sigcxx::SLOT;

Observer::Observer()
    : Trackable(), test0_count_(0), test1_count_(0), test2_count_(0) {

}

Observer::~Observer() {

}

void Observer::OnTest0(SLOT /* slot */) {
  test0_count_++;
}

void Observer::OnTest1IntegerParam(SLOT /* slot */, int n) {
  test1_count_++;
}

void Observer::OnTest2IntegerParams(SLOT /* slot */, int n1, int n2) {
  test2_count_++;
}

void Observer::OnTestDestroy(SLOT slot) {
  UnbindOnce(slot); // call this before delete self
  delete this;
}

void Observer::OnTestUnbindOnceAt5(SLOT slot) {
  test0_count_++;
  if (test0_count_ >= 5) {
    UnbindOnce(slot);
  }
}

void Observer::OnTestUnbindAllAt5(SLOT slot) {
  test0_count_++;
  if (test0_count_ >= 5) {
    UnbindAll(slot);
  }
}

void Observer::OnTestUnbindAllMethodAt5(SLOT slot) {
  test0_count_++;
  if (test0_count_ >= 5) {
    UnbindAll(slot, &Observer::OnTestUnbindAllMethodAt5);
  }
}

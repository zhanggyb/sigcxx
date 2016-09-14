#include "observer.hpp"

Observer::Observer()
: Trackable(), test0_count_(0), test1_count_(0), test2_count_(0) {

}

Observer::~Observer() {

}

void Observer::OnTest0(const Sender *sender) {
  test0_count_++;
}

void Observer::OnTest1IntegerParam(const Sender *sender, int n) {
  test1_count_++;
}

void Observer::OnTest2IntegerParams(const Sender *sender, int n1, int n2) {
  test2_count_++;
}

void Observer::OnTestDestroy(const Sender *sender) {
  UnbindOnce(sender); // call this before delete self
  delete this;
}
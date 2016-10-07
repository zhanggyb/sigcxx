#include "subject.hpp"

using sigcxx::Trackable;
using sigcxx::SLOT;

Subject::Subject()
    : Trackable(),
      test1_count_(0), test2_count_(0) {

}

Subject::~Subject() {

}

void Subject::OnTest1IntegerParam(SLOT slot, int n) {
  test1_count_++;
}

void Subject::OnTest2IntegerParams(SLOT slot, int n1, int n2) {
  test2_count_++;
}
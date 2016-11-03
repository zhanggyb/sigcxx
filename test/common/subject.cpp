#include "subject.hpp"

using sigcxx::Trackable;
using sigcxx::SLOT;

Subject::Subject()
    : Trackable(),
      test1_count_(0), test2_count_(0) {

}

Subject::~Subject() {

}

void Subject::OnTest1IntegerParam(int n, SLOT slot) {
  test1_count_++;
}

void Subject::OnTest2IntegerParams(int n1, int n2, SLOT slot) {
  test2_count_++;
}
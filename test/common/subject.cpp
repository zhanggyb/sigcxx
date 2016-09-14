#include "subject.hpp"

Subject::Subject()
    : Trackable(),
      test1_count_(0), test2_count_(0) {

}

Subject::~Subject() {

}

void Subject::OnTest1IntegerParam(const Sender *sender, int n) {
  test1_count_++;
}

void Subject::OnTest2IntegerParams(const Sender *sender, int n1, int n2) {
  test2_count_++;
}
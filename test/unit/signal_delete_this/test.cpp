#include "test.hpp"
#include <sigcxx/sigcxx.hpp>
#include <iostream>

using namespace sigcxx;

class Button {
 public:

  Button() {}

  ~Button() {}

  SignalRef<> clicked() {
    return clicked_;
  }

  Signal<> clicked_;
};

class TitleBar : public Trackable {
 public:

  TitleBar() {
    button_.clicked().Connect(this, &TitleBar::OnClicked);
  }

  virtual ~TitleBar() {}

  void OnClicked(__SLOT__) {
    action_(1);
  }

  SignalRef<int> action() {
    return action_;
  }

  Signal<int> action_;

  Button button_;
};

class Window : public Trackable {
 public:

  Window() {
    titlebar_.action().Connect(this, &Window::OnClose);
  }

  virtual ~Window() {}

  void OnClose(int i, __SLOT__) {
//    std::cout << "This object is deleted" << std::endl;
    delete this;
  }

  TitleBar titlebar_;
};

Test::Test()
    : testing::Test() {
}

Test::~Test() {

}

/**
 * @brief This test case simulate a window with a title bar and close button
 */
TEST_F(Test, push_back1) {
  for (int i = 0; i < 10000; i++) {
    Window *win = new Window;
    win->titlebar_.button_.clicked_();
  }

  // check the stdout and no segment fault
  ASSERT_TRUE(true);
}

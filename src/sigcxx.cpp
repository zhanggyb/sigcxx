/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Freeman Zhang <zhanggyb@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <sigcxx/sigcxx.hpp>

namespace sigcxx {

namespace details {

Binding::~Binding() {
  if (previous) previous->next = next;
  if (next) next->previous = previous;

  if (trackable_object) {
    if (!previous)
      trackable_object->first_binding_ = next;
    if (!next)
      trackable_object->last_binding_ = previous;
  }

  previous = nullptr;
  next = nullptr;

  if (token) {
#ifdef DEBUG
    assert(token->binding == this);
#endif
    token->binding = nullptr;
    delete token;
    token = nullptr;
  }
}

Token::~Token() {
  if (trackable_object) trackable_object->AuditDestroyingToken(this);

  if (previous) previous->next = next;
  if (next) next->previous = previous;

  previous = nullptr;
  next = nullptr;

  if (binding) {
#ifdef DEBUG
    assert(binding->token == this);
#endif
    binding->token = nullptr;
    delete binding;
    binding = nullptr;
  }
}

}  // namespace details

Trackable::~Trackable() {
  UnbindAll();
}

void Trackable::Unbind(SLOT slot) {
  // (sender && sender->token_->binding->trackable_object == this) is always true
  if (slot != nullptr) {
    details::Token *tmp = slot->token_;
    const_cast<Slot *>(slot)->token_ = slot->token_->next;
    delete tmp;
    const_cast<Slot *>(slot)->skip_ = true;
  }
}

void Trackable::UnbindAll(SLOT slot) {
  // (slot && slot->token_->binding->trackable_object == this) is always true

  if (slot == nullptr) {
    details::Binding *tmp = nullptr;
    details::Binding *p = first_binding_;

    while (p) {
      tmp = p->next;
      delete p;
      p = tmp;
    }
  } else {
    details::Token *tmp = nullptr;
    details::Token *p = nullptr;

    p = slot->token_;
    while (p) {
      tmp = p->previous;
      if (p->binding->trackable_object == this) {
        if (p == slot->token_) {
          const_cast<Slot *>(slot)->token_ = slot->token_->next;
          const_cast<Slot *>(slot)->skip_ = true;
        }
        delete p;
      }
      p = tmp;
    }

    p = slot->token_;
    while (p) {
      tmp = p->next;
      if (p->binding->trackable_object == this) {
        if (p == slot->token_) {
          const_cast<Slot *>(slot)->token_ = slot->token_->next;
          const_cast<Slot *>(slot)->skip_ = true;
        }
        delete p;
      }
      p = tmp;
    }
  }
}

std::size_t Trackable::CountBindings() const {
  std::size_t count = 0;
  for (details::Binding *p = first_binding_; p; p = p->next) {
    count++;
  }
  return count;
}

void Trackable::PushBackBinding(details::Binding *node) {
#ifdef DEBUG
  assert(node->trackable_object == nullptr);
#endif

  if (last_binding_) {
    last_binding_->next = node;
    node->previous = last_binding_;
  } else {
#ifdef DEBUG
    assert(first_binding_ == nullptr);
#endif
    node->previous = nullptr;
    first_binding_ = node;
  }
  last_binding_ = node;
  node->next = nullptr;
  node->trackable_object = this;
}

void Trackable::PushFrontBinding(details::Binding *node) {
#ifdef DEBUG
  assert(node->trackable_object == nullptr);
#endif

  if (first_binding_) {
    first_binding_->previous = node;
    node->next = first_binding_;
  } else {
#ifdef DEBUG
    assert(last_binding_ == nullptr);
#endif
    node->next = nullptr;
    last_binding_ = node;
  }
  first_binding_ = node;

  node->previous = nullptr;
  node->trackable_object = this;
}

void Trackable::InsertBinding(int index, details::Binding *node) {
#ifdef DEBUG
  assert(node->trackable_object == nullptr);
#endif

  if (first_binding_ == nullptr) {
#ifdef DEBUG
    assert(last_binding_ == nullptr);
#endif
    node->next = nullptr;
    last_binding_ = node;
    first_binding_ = node;
    node->previous = nullptr;
  } else {
    if (index >= 0) {

      details::Binding *p = first_binding_;
#ifdef DEBUG
      assert(p != nullptr);
#endif

      while (p && (index > 0)) {
        p = p->next;
        index--;
      }

      if (p) {  // insert before p

        node->previous = p->previous;
        node->next = p;

        if (p->previous) p->previous->next = node;
        else first_binding_ = node;

        p->previous = node;

      } else {  // push back

        last_binding_->next = node;
        node->previous = last_binding_;
        last_binding_ = node;
        node->next = nullptr;

      }

    } else {

      details::Binding *p = last_binding_;
#ifdef DEBUG
      assert(p != nullptr);
#endif

      while (p && (index < -1)) {
        p = p->previous;
        index++;
      }

      if (p) {  // insert after p

        node->next = p->next;
        node->previous = p;

        if (p->next) p->next->previous = node;
        else last_binding_ = node;

        p->next = node;

      } else {  // push front

        first_binding_->previous = node;
        node->next = first_binding_;
        first_binding_ = node;
        node->previous = nullptr;

      }

    }
  }
  node->trackable_object = this;
}

}  // namespace sigcxx

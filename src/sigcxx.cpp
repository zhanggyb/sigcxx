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
    if (nullptr == previous) trackable_object->first_binding_ = next;
    if (nullptr == next) trackable_object->last_binding_ = previous;
  }

  if (token) {
#ifdef DEBUG
    assert(token->binding == this);
#endif
    token->binding = nullptr;
    delete token;
  }
}

Token::~Token() {
  if (trackable_object) trackable_object->AuditDestroyingToken(this);

  if (slot) {
    // This token is emitting
    slot->token_ = next;
    slot->skip_ = true;
  }

  if (previous) previous->next = next;
  if (next) next->previous = previous;

  if (binding) {
#ifdef DEBUG
    assert(binding->token == this);
#endif
    binding->token = nullptr;
    delete binding;
  }
}

}  // namespace details

Trackable::~Trackable() {
  UnbindAll();
}

void Trackable::Unbind(SLOT slot) {
  if (slot->token_->binding->trackable_object == this) {
    details::Token* tmp = slot->token_;
    slot->token_ = slot->token_->next;
    slot->skip_ = true;

    tmp->slot = nullptr;
    delete tmp;
  }
}

void Trackable::UnbindAll() {
  details::Binding *tmp = nullptr;
  details::Binding *it = last_binding_;

  while (it) {
    tmp = it;
    it = it->previous;
    delete tmp;
  }
}

std::size_t Trackable::CountBindings() const {
  std::size_t count = 0;
  for (details::Binding *it = first_binding_; it; it = it->next) {
    count++;
  }
  return count;
}

void Trackable::PushBackBinding(details::Binding *node) {
#ifdef DEBUG
  assert(nullptr == node->trackable_object);
#endif

  if (last_binding_) {
    last_binding_->next = node;
    node->previous = last_binding_;
  } else {
#ifdef DEBUG
    assert(nullptr == first_binding_);
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
  assert(nullptr == node->trackable_object);
#endif

  if (first_binding_) {
    first_binding_->previous = node;
    node->next = first_binding_;
  } else {
#ifdef DEBUG
    assert(nullptr == last_binding_);
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
  assert(nullptr == node->trackable_object);
#endif

  if (nullptr == first_binding_) {
#ifdef DEBUG
    assert(nullptr == last_binding_);
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
      assert(p);
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

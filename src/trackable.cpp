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

#include <cppevent/connection.hpp>
#include <cppevent/trackable.hpp>
#include <cassert>

namespace CppEvent {

Trackable::~Trackable ()
{
  Clear();
}

void Trackable::Clear ()
{
  Connection* tmp = 0;
  Connection* p = head_connection_;

  while (p) {

    tmp = p->next_;

    p->previous_ = 0;
    p->next_ = 0;
    p->trackable_ = 0;

    // TODO: disconnect before delete
    delete p;

    p = tmp;

  }

  connection_count_ = 0;
}

void Trackable::PushBackConnection (Connection* node)
{
  assert(node->trackable_ == 0);

  if (tail_connection_) {
    tail_connection_->next_ = node;
    node->previous_ = tail_connection_;
  } else {
    assert(head_connection_ == 0);
    node->previous_ = 0;
    head_connection_ = node;
  }
  tail_connection_ = node;
  node->next_ = 0;
  node->trackable_ = this;
  connection_count_++;
}

void Trackable::PushFrontConnection (Connection* node)
{
  assert(node->trackable_ == 0);

  if (head_connection_) {
    head_connection_->previous_ = node;
    node->next_ = head_connection_;
  } else {
    assert(tail_connection_ == 0);
    node->next_ = 0;
    tail_connection_ = node;
  }
  head_connection_ = node;

  node->previous_ = 0;
  node->trackable_ = this;
  connection_count_++;
}

void Trackable::AuditDestroyingConnection (Connection* node)
{
  // TODO: override this
}

void Trackable::InsertConnection (int index, Connection* node)
{
  assert(node->trackable_ == 0);

  if (head_connection_ == 0) {
    assert(tail_connection_ == 0);

    node->next_ = 0;
    tail_connection_ = node;
    head_connection_ = node;
    node->previous_ = 0;
  } else {
    if (index > 0) {

      Connection* p = head_connection_;

      while (p && (index > 0)) {
        if (p->next_ == 0) break;
        p = p->next_;
        index--;
      }

      if (index == 0) {  // insert
        node->previous_ = p->previous_;
        node->next_ = p;
        p->previous_->next_ = node;
        p->previous_ = node;
      } else {  // same as push back
        assert(p == tail_connection_);
        tail_connection_->next_ = node;
        node->previous_ = tail_connection_;
        tail_connection_ = node;
        node->next_ = 0;
      }

    } else {  // same as push front
      head_connection_->previous_ = node;
      node->next_ = head_connection_;
      head_connection_ = node;
      node->previous_ = 0;
    }
  }
  node->trackable_ = this;
  connection_count_++;
}

} // namespace CppEvent

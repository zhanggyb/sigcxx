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
#include <cppevent/abstract-trackable.hpp>
#include <cassert>

namespace CppEvent {

AbstractTrackable::~AbstractTrackable ()
{
  // MUST call RemoveAllConnections() in sub class destructor
  assert(connection_count_ == 0);
}

void AbstractTrackable::PushBackConnection (Connection* node)
{
  assert(node->trackable_object_ == 0);

  if (tail_connection_) {
    tail_connection_->next_connection_ = node;
    node->previous_connection_ = tail_connection_;
  } else {
    assert(head_connection_ == 0);
    node->previous_connection_ = 0;
    head_connection_ = node;
  }
  tail_connection_ = node;
  node->next_connection_ = 0;
  node->trackable_object_ = this;
  connection_count_++;
}

void AbstractTrackable::PushFrontConnection (Connection* node)
{
  assert(node->trackable_object_ == 0);

  if (head_connection_) {
    head_connection_->previous_connection_ = node;
    node->next_connection_ = head_connection_;
  } else {
    assert(tail_connection_ == 0);
    node->next_connection_ = 0;
    tail_connection_ = node;
  }
  head_connection_ = node;

  node->previous_connection_ = 0;
  node->trackable_object_ = this;
  connection_count_++;
}

void AbstractTrackable::AuditDestroyingConnection (Connection* node)
{
  // TODO: override this
}

void AbstractTrackable::InsertConnection (int index, Connection* node)
{
  assert(node->trackable_object_ == 0);

  if (head_connection_ == 0) {
    assert(tail_connection_ == 0);

    node->next_connection_ = 0;
    tail_connection_ = node;
    head_connection_ = node;
    node->previous_connection_ = 0;
  } else {
    if (index > 0) {

      Connection* p = head_connection_;

      while (p && (index > 0)) {
        if (p->next_connection_ == 0) break;
        p = p->next_connection_;
        index--;
      }

      if (index == 0) {  // insert
        node->previous_connection_ = p->previous_connection_;
        node->next_connection_ = p;
        p->previous_connection_->next_connection_ = node;
        p->previous_connection_ = node;
      } else {  // same as push back
        assert(p == tail_connection_);
        tail_connection_->next_connection_ = node;
        node->previous_connection_ = tail_connection_;
        tail_connection_ = node;
        node->next_connection_ = 0;
      }

    } else {  // same as push front
      head_connection_->previous_connection_ = node;
      node->next_connection_ = head_connection_;
      head_connection_ = node;
      node->previous_connection_ = 0;
    }
  }
  node->trackable_object_ = this;
  connection_count_++;
}

void AbstractTrackable::RemoveAllConnections ()
{
  Connection* tmp = 0;
  Connection* p = head_connection_;

  while (p) {
    tmp = p->next_connection_;
    delete p;
    p = tmp;
  }

  connection_count_ = 0;
}

} // namespace CppEvent

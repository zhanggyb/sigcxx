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

#ifdef DEBUG
#include <cassert>
#include <iostream> // for debug
#endif

namespace CppEvent {

Connection::~Connection ()
{
  if (trackable_object_) {

    trackable_object_->AuditDestroyingConnection(this);

    if (previous_connection_)
      previous_connection_->next_connection_ = next_connection_;
    else
      trackable_object_->head_connection_ = next_connection_;

    if (next_connection_)
      next_connection_->previous_connection_ = previous_connection_;
    else
      trackable_object_->tail_connection_ = previous_connection_;

    trackable_object_->connection_count_--;
    trackable_object_ = 0;

  } else {

    if (previous_connection_) previous_connection_->next_connection_ =
        next_connection_;
    if (next_connection_) next_connection_->previous_connection_ =
        previous_connection_;

  }

  previous_connection_ = 0;
  next_connection_ = 0;

  if (upstream_connection_) {
#ifdef DEBUG
    assert(upstream_connection_->downstream_connection_ == this);
#endif
    upstream_connection_->downstream_connection_ = 0;
    delete upstream_connection_;
    upstream_connection_ = 0;
  }

  if (downstream_connection_) {
#ifdef DEBUG
    assert(downstream_connection_->upstream_connection_ == this);
#endif
    downstream_connection_->upstream_connection_ = 0;
    delete downstream_connection_;
    downstream_connection_ = 0;
  }
}

bool Connection::Link (Connection* upstream, Connection* downstream)
{
  if (upstream == downstream) return false;
  if ((upstream == 0) || (downstream == 0)) return false;

#ifdef DEBUG
  assert((upstream->downstream_connection_ == 0) && (downstream->upstream_connection_ == 0));
#endif

  upstream->downstream_connection_ = downstream;
  downstream->upstream_connection_ = upstream;

  return true;
}

}

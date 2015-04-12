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

#include <cppevent/slot.hpp>
#include <cppevent/trackable.hpp>

#ifdef DEBUG
#include <cassert>
#include <iostream> // for debug
#endif

namespace CppEvent {

Slot::~Slot ()
{
  if (trackable_object_) {

    trackable_object_->AuditDestroyingSlot(this);

    if (previous_)
      previous_->next_ = next_;
    else
      trackable_object_->head_slot_ = next_;

    if (next_)
      next_->previous_ = previous_;
    else
      trackable_object_->tail_slot_ = previous_;

    trackable_object_->slot_count_--;
    trackable_object_ = 0;

  } else {

    if (previous_) previous_->next_ =
        next_;
    if (next_) next_->previous_ =
        previous_;

  }

  previous_ = 0;
  next_ = 0;

  if (upstream_) {
#ifdef DEBUG
    assert(upstream_->downstream_ == this);
#endif
    upstream_->downstream_ = 0;
    delete upstream_;
    upstream_ = 0;
  }

  if (downstream_) {
#ifdef DEBUG
    assert(downstream_->upstream_ == this);
#endif
    downstream_->upstream_ = 0;
    delete downstream_;
    downstream_ = 0;
  }
}

bool Slot::Link (Slot* upstream, Slot* downstream)
{
  if (upstream == downstream) return false;
  if ((upstream == 0) || (downstream == 0)) return false;

#ifdef DEBUG
  assert((upstream->downstream_ == 0) && (downstream->upstream_ == 0));
#endif

  upstream->downstream_ = downstream;
  downstream->upstream_ = upstream;

  return true;
}

}

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

#include <cppevent/abstract-trackable.hpp>
#include <cppevent/slot.hpp>

#ifdef DEBUG
#include <cassert>
#endif

namespace CppEvent {

AbstractTrackable::~AbstractTrackable ()
{
  // MUST call RemoveAllConnections() in sub class destructor
#ifdef DEBUG
  assert(slot_count_ == 0);
#endif
}

void AbstractTrackable::PushBackSlot (Slot* node)
{
#ifdef DEBUG
  assert(node->trackable_object_ == 0);
#endif

  if (tail_slot_) {
    tail_slot_->next_ = node;
    node->previous_ = tail_slot_;
  } else {
#ifdef DEBUG
    assert(head_slot_ == 0);
#endif
    node->previous_ = 0;
    head_slot_ = node;
  }
  tail_slot_ = node;
  node->next_ = 0;
  node->trackable_object_ = this;
  slot_count_++;
}

void AbstractTrackable::PushFrontSlot (Slot* node)
{
#ifdef DEBUG
  assert(node->trackable_object_ == 0);
#endif

  if (head_slot_) {
    head_slot_->previous_ = node;
    node->next_ = head_slot_;
  } else {
#ifdef DEBUG
    assert(tail_slot_ == 0);
#endif
    node->next_ = 0;
    tail_slot_ = node;
  }
  head_slot_ = node;

  node->previous_ = 0;
  node->trackable_object_ = this;
  slot_count_++;
}

void AbstractTrackable::AuditDestroyingSlot (Slot* node)
{
  // TODO: override this
}

void AbstractTrackable::InsertSlot (int index, Slot* node)
{
#ifdef DEBUG
  assert(node->trackable_object_ == 0);
#endif

  if (head_slot_ == 0) {
#ifdef DEBUG
    assert(tail_slot_ == 0);
#endif

    node->next_ = 0;
    tail_slot_ = node;
    head_slot_ = node;
    node->previous_ = 0;
  } else {
    if (index > 0) {

      Slot* p = head_slot_;

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
#ifdef DEBUG
        assert(p == tail_slot_);
#endif
        tail_slot_->next_ = node;
        node->previous_ = tail_slot_;
        tail_slot_ = node;
        node->next_ = 0;
      }

    } else {  // same as push front
      head_slot_->previous_ = node;
      node->next_ = head_slot_;
      head_slot_ = node;
      node->previous_ = 0;
    }
  }
  node->trackable_object_ = this;
  slot_count_++;
}

void AbstractTrackable::RemoveAllSlots ()
{
  Slot* tmp = 0;
  Slot* p = head_slot_;

  while (p) {
    tmp = p->next_;
    delete p;
    p = tmp;
  }

  slot_count_ = 0;
}

} // namespace CppEvent

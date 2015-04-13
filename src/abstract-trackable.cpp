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
#include <cppevent/signal.hpp>
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
  assert(node->trackable_object == 0);
#endif

  if (tail_slot_) {
    tail_slot_->next = node;
    node->previous = tail_slot_;
  } else {
#ifdef DEBUG
    assert(head_slot_ == 0);
#endif
    node->previous = 0;
    head_slot_ = node;
  }
  tail_slot_ = node;
  node->next = 0;
  node->trackable_object = this;
  slot_count_++;
}

void AbstractTrackable::PushFrontSlot (Slot* node)
{
#ifdef DEBUG
  assert(node->trackable_object == 0);
#endif

  if (head_slot_) {
    head_slot_->previous = node;
    node->next = head_slot_;
  } else {
#ifdef DEBUG
    assert(tail_slot_ == 0);
#endif
    node->next = 0;
    tail_slot_ = node;
  }
  head_slot_ = node;

  node->previous = 0;
  node->trackable_object = this;
  slot_count_++;
}

void AbstractTrackable::AuditDestroyingSlot (Slot* node)
{
  // TODO: override this
}

void AbstractTrackable::InsertSlot (int index, Slot* node)
{
#ifdef DEBUG
  assert(node->trackable_object == 0);
#endif

  if (head_slot_ == 0) {
#ifdef DEBUG
    assert(tail_slot_ == 0);
#endif

    node->next = 0;
    tail_slot_ = node;
    head_slot_ = node;
    node->previous = 0;
  } else {
    if (index > 0) {

      Slot* p = head_slot_;

      while (p && (index > 0)) {
        if (p->next == 0) break;
        p = p->next;
        index--;
      }

      if (index == 0) {  // insert
        node->previous = p->previous;
        node->next = p;
        p->previous->next = node;
        p->previous = node;
      } else {  // same as push back
#ifdef DEBUG
        assert(p == tail_slot_);
#endif
        tail_slot_->next = node;
        node->previous = tail_slot_;
        tail_slot_ = node;
        node->next = 0;
      }

    } else {  // same as push front
      head_slot_->previous = node;
      node->next = head_slot_;
      head_slot_ = node;
      node->previous = 0;
    }
  }
  node->trackable_object = this;
  slot_count_++;
}

void AbstractTrackable::RemoveAllSlots ()
{
  Slot* tmp = 0;
  Slot* p = head_slot_;

  while (p) {
    tmp = p->next;
    delete p;
    p = tmp;
  }

  slot_count_ = 0;
}

bool AbstractTrackable::Link (Signal* source, Slot* consumer)
{
  if ((source == 0) || (consumer == 0)) return false;

#ifdef DEBUG
  assert((source->slot == 0) && (consumer->signal == 0));
#endif

  source->slot = consumer;
  consumer->signal = source;

  return true;
}

} // namespace CppEvent

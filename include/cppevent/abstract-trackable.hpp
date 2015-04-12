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

#pragma once

namespace CppEvent {

class Slot;

/**
 * @brief Abstract class for event
 */
class AbstractTrackable
{
  friend class Slot;

public:

  inline AbstractTrackable ()
      : head_slot_(0), tail_slot_(0), slot_count_(0)
  {
  }

  virtual ~AbstractTrackable ();

  inline int slot_count () const
  {
    return slot_count_;
  }

protected:

  virtual void AuditDestroyingSlot (Slot* node) = 0;

  void PushBackSlot (Slot* node);

  void PushFrontSlot (Slot* node);

  void InsertSlot (int index, Slot* node);

  void RemoveAllSlots ();

  inline Slot* head_slot () const
  {
    return head_slot_;
  }

  inline Slot* tail_slot () const
  {
    return tail_slot_;
  }

  static inline void add_slot (AbstractTrackable* trackable,
                                     Slot* conn)
  {
    trackable->PushBackSlot(conn);
  }

  static inline void insert_slot (AbstractTrackable* trackable,
                                        Slot* conn,
                                        int index = 0)
  {
    trackable->InsertSlot(index, conn);
  }

private:

  Slot* head_slot_;
  Slot* tail_slot_;
  int slot_count_;
};

}

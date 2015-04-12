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

// forward declaration
class AbstractTrackable;

/**
 * @brief The abstract event node
 */
class Slot
{
public:

  inline Slot ()
  : trackable_object_(0),
    previous_(0),
    next_(0),
    upstream_(0),
    downstream_(0)
  {
  }

  virtual ~Slot ();

  static bool Link (Slot* upstream, Slot* downstream);

  inline AbstractTrackable* trackable_object () const
  {
    return trackable_object_;
  }

  inline Slot* previous () const
  {
    return previous_;
  }

  inline Slot* next () const
  {
    return next_;
  }

  inline Slot* upstream () const
  {
    return upstream_;
  }

  inline Slot* downstream () const
  {
    return downstream_;
  }

private:

  friend class AbstractTrackable;

  AbstractTrackable* trackable_object_;
  Slot* previous_;
  Slot* next_;
  Slot* upstream_;
  Slot* downstream_;
};

} // namespace CppEvent

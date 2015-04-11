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
class Connection
{
public:

  inline Connection ()
  : trackable_object_(0),
    previous_connection_(0),
    next_connection_(0),
    upstream_connection_(0),
    downstream_connection_(0)
  {
  }

  virtual ~Connection ();

  static bool Link (Connection* upstream, Connection* downstream);

  inline AbstractTrackable* trackable_object () const
  {
    return trackable_object_;
  }

  inline Connection* previous_connection () const
  {
    return previous_connection_;
  }

  inline Connection* next_connection () const
  {
    return next_connection_;
  }

  inline Connection* upstream_connection () const
  {
    return upstream_connection_;
  }

  inline Connection* downstream_connection () const
  {
    return downstream_connection_;
  }

private:

  friend class AbstractTrackable;

  AbstractTrackable* trackable_object_;
  Connection* previous_connection_;
  Connection* next_connection_;
  Connection* upstream_connection_;
  Connection* downstream_connection_;
};

} // namespace CppEvent

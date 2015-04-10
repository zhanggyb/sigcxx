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
//template<typename ... ParamTypes> class Connection;
//template<typename ReturnType, typename ... ParamTypes> struct ConnectionNode;
 class Connection;

/**
 * @brief Abstract class for event
 */
class AbstractTrackable
{
  friend class Connection;

public:

  inline AbstractTrackable ()
      : head_connection_(0), tail_connection_(0), connection_count_(0)
  {
  }

  virtual ~AbstractTrackable ();

  inline int count () const
  {
    return connection_count_;
  }

  void Clear ();

protected:

  virtual void AuditDestroyingConnection (Connection* node) = 0;

  void PushBackConnection (Connection* node);

  void PushFrontConnection (Connection* node);

  void InsertConnection (int index, Connection* node);

  inline Connection* head_connection () const
  {
    return head_connection_;
  }

  inline Connection* tail_connection () const
  {
    return tail_connection_;
  }

private:

  Connection* head_connection_;
  Connection* tail_connection_;
  int connection_count_;
};

}

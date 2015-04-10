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

#include <cppevent/delegate.hpp>
#include <cppevent/invokable-connection.hpp>

namespace CppEvent {

template<typename ... ParamTypes>
class DelegateConnection : public InvokableConnection < ParamTypes... >
{
public:

  inline DelegateConnection();

  inline DelegateConnection(const Delegate<void, ParamTypes...>& d);

  virtual ~DelegateConnection();

  virtual void Invoke(ParamTypes... Args) override;

private:

  Delegate<void, ParamTypes...> delegate_;

  //ConnectionNode<ReturnType, ParamTypes...>* connection_node;
};

template<typename ... ParamTypes>
inline DelegateConnection<ParamTypes...>::DelegateConnection()
  : InvokableConnection<ParamTypes...>()
{
}

template<typename ... ParamTypes>
inline DelegateConnection<ParamTypes...>::DelegateConnection(const Delegate<void, ParamTypes...>& d)
  : InvokableConnection<ParamTypes...>(), delegate_(d)
{
}

template<typename ... ParamTypes>
DelegateConnection<ParamTypes...>::~DelegateConnection()
{
  /*
  if (connection_node) {
  //assert(connection_node->delegate_node == this);
  connection_node->delegate_node = 0;
  delete connection_node;
  connection_node = 0;
  }
  */
}

template<typename ... ParamTypes>
void DelegateConnection<ParamTypes...>::Invoke(ParamTypes... Args)
{
  if (delegate_) delegate_(Args...);
}

} // namespace CppEvent

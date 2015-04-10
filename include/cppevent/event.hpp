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

#include <cppevent/delegate-connection.hpp>
#include <cppevent/trackable.hpp>

namespace CppEvent {

template<typename ... ParamTypes>
class Event: public Trackable
{
public:

  inline Event ();

  virtual ~Event ();

  template<typename T, void (T::*TMethod) (ParamTypes...)>
  void connect (T* obj);

  template<typename T, void (T::*TMethod) (ParamTypes...) const>
  void connect (T* obj);

  template<typename T, void (T::*TMethod) (ParamTypes...)>
  void disconnect (T* obj);

  virtual void Invoke (ParamTypes ... Args);

protected:

  virtual void AuditDestroyingConnection (Connection* node);

private:

  Connection* v_;  // a node pointer to iterate through all nodes in fire()

};

template<typename ... ParamTypes>
inline Event<ParamTypes...>::Event ()
    : Trackable(), v_(0)
{
}

template<typename ... ParamTypes>
Event<ParamTypes...>::~Event ()
{
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...)>
void Event<ParamTypes...>::connect (T* obj)
{
  Delegate<void, ParamTypes...> d =
      Delegate<void, ParamTypes...>::template from_method<T, TMethod>(obj);
  this->PushBackConnection(new DelegateConnection<ParamTypes...>(d));
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...) const>
void Event<ParamTypes...>::connect (T* obj)
{
  Delegate<void, ParamTypes...> d =
      Delegate<void, ParamTypes...>::template from_const_method<T, TMethod>(
          obj);
  this->PushBackConnection(new DelegateConnection<ParamTypes...>(d));
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...)>
void Event<ParamTypes...>::disconnect (T* obj)
{
  // TODO
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Invoke (ParamTypes ... Args)
{
  v_ = 0;
  for (v_ = this->head_connection(); v_; v_ = v_->next()) {
    static_cast<DelegateConnection<ParamTypes...>*>(v_)->Invoke(Args...);
  }
  v_ = 0;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::AuditDestroyingConnection (Connection* node)
{
  if (node == v_) {
    v_ = v_->next();
  }
}

}

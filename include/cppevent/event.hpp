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

#include <cppevent/abstract-trackable.hpp>
#include <cppevent/trackable.hpp>
#include <cppevent/delegate-slot.hpp>
#include <cppevent/event-slot.hpp>

namespace CppEvent {

template<typename ... ParamTypes>
class Event: public AbstractTrackable
{
public:

  inline Event ();

  virtual ~Event ();

  /**
   * @brief Connect this event to a method of trackable object
   */
  template<typename T, void (T::*TMethod) (ParamTypes...)>
  void Connect (T* obj);

  /**
   * @brief Connect this event to a const method of trackable object
   */
  template<typename T, void (T::*TMethod) (ParamTypes...) const>
  void Connect (T* obj);

  void Connect (Event<ParamTypes...>& other);

  template<typename T, void (T::*TMethod) (ParamTypes...)>
  void DisconnectOne (T* obj);

  template<typename T, void (T::*TMethod) (ParamTypes...)>
  void DisconnectAll (T* obj);

  template<typename T, void (T::*TMethod) (ParamTypes...) const>
  void DisconnectOne (T* obj);

  template<typename T, void (T::*TMethod) (ParamTypes...) const>
  void DisconnectAll (T* obj);

  void DisconnectOne (Event<ParamTypes...>& other);

  void DisconnectAll (Event<ParamTypes...>& other);

  void DisconnectAll ();

  virtual void Invoke (ParamTypes ... Args);

protected:

  virtual void AuditDestroyingSlot (Slot* node) final;

private:

  Slot* iterator_;  // a pointer to iterate through all connections
  bool iterator_removed_;

};

template<typename ... ParamTypes>
inline Event<ParamTypes...>::Event ()
    : AbstractTrackable(), iterator_(0), iterator_removed_(false)
{
}

template<typename ... ParamTypes>
Event<ParamTypes...>::~Event ()
{
    RemoveAllSlots();
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...)>
void Event<ParamTypes...>::Connect (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {

    Slot* downstream = new Slot;

    Delegate<void, ParamTypes...> d =
        Delegate<void, ParamTypes...>::template from_method<T, TMethod>(obj);
    DelegateSlot<ParamTypes...>* upstream = new DelegateSlot<
        ParamTypes...>(d);

    Slot::Link(upstream, downstream);

    this->PushBackSlot(upstream);
    add_slot(trackable_object, downstream);

    return;
  }
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...) const>
void Event<ParamTypes...>::Connect (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {

    Slot* downstream = new Slot;

    Delegate<void, ParamTypes...> d =
        Delegate<void, ParamTypes...>::template from_const_method<T, TMethod>(
            obj);
    DelegateSlot<ParamTypes...>* upstream = new DelegateSlot<
        ParamTypes...>(d);

    Slot::Link(upstream, downstream);

    this->PushBackSlot(upstream);
    add_slot(trackable_object, downstream);

    return;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Connect (Event<ParamTypes...>& other)
{
  EventSlot<ParamTypes...>* upstream = new EventSlot<ParamTypes...>(
      &other);
  InvokableSlot<ParamTypes...>* downstream = new InvokableSlot<
      ParamTypes...>;
  Slot::Link(upstream, downstream);
  this->PushBackSlot(upstream);
  add_slot(&other, downstream);
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...)>
void Event<ParamTypes...>::DisconnectOne (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateSlot<ParamTypes...>* conn = 0;
    for (Slot* p = tail_slot(); p; p = p->previous()) {
      conn = dynamic_cast<DelegateSlot<ParamTypes...>*>(p);
      if (conn && (conn->delegate().template equal<T, TMethod>(obj))) {
        delete conn;
        break;
      }
    }

    return;
  }
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...)>
void Event<ParamTypes...>::DisconnectAll (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateSlot<ParamTypes...>* conn = 0;
    for (Slot* p = tail_slot(); p; p = p->previous()) {
      conn = dynamic_cast<DelegateSlot<ParamTypes...>*>(p);
      if (conn && (conn->delegate().template equal<T, TMethod>(obj)))
        delete conn;
    }

    return;
  }
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...) const>
void Event<ParamTypes...>::DisconnectOne (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateSlot<ParamTypes...>* conn = 0;
    for (Slot* p = tail_slot(); p; p = p->previous()) {
      conn = dynamic_cast<DelegateSlot<ParamTypes...>*>(p);
      if (conn && (conn->delegate().template equal<T, TMethod>(obj))) {
        delete conn;
        break;
      }
    }
    return;
  }
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...) const>
void Event<ParamTypes...>::DisconnectAll (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateSlot<ParamTypes...>* conn = 0;
    for (Slot* p = tail_slot(); p; p = p->previous()) {
      conn = dynamic_cast<DelegateSlot<ParamTypes...>*>(p);
      if (conn && (conn->delegate().template equal<T, TMethod>(obj)))
        delete conn;
    }
    return;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectOne (Event<ParamTypes...>& other)
{
  EventSlot<ParamTypes...>* conn = 0;
  for (Slot* p = tail_slot(); p; p = p->previous()) {
    conn = dynamic_cast<EventSlot<ParamTypes...>*>(p);
    if (conn && (conn->event() == (&other))) {
      delete conn;
      break;
    }
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAll (Event<ParamTypes...>& other)
{
  EventSlot<ParamTypes...>* conn = 0;
  for (Slot* p = tail_slot(); p; p = p->previous()) {
    conn = dynamic_cast<EventSlot<ParamTypes...>*>(p);
    if (conn && (conn->event() == (&other))) delete conn;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAll ()
{
  RemoveAllSlots();
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Invoke (ParamTypes ... Args)
{
  iterator_ = this->head_slot();
  while (iterator_) {
    static_cast<InvokableSlot<ParamTypes...>*>(iterator_)->Invoke(Args...);

    // check if iterator was deleted when being invoked
    // (iterator_removed_ was set via the virtual AuditDestroyingConnection()
    if (iterator_removed_)
      iterator_removed_ = false;
    else
      iterator_ = iterator_->next();
  }
  iterator_ = 0;
  iterator_removed_ = false;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::AuditDestroyingSlot (Slot* node)
{
  if (node == iterator_) {
    iterator_removed_ = true;
    iterator_ = iterator_->next();
  }
}

}

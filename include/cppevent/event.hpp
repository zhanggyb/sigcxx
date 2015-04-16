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
#include <cppevent/delegate-invoker.hpp>
#include <cppevent/event-invoker.hpp>
#include <cppevent/slot.hpp>
#include <cppevent/trackable.hpp>

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
  template<typename T>
  void Connect (T* obj, void (T::*TMethod) (ParamTypes...));

  void Connect (Event<ParamTypes...>& other);

  template<typename T>
  void DisconnectOne (T* obj, void (T::*TMethod) (ParamTypes...));

  template<typename T>
  void DisconnectAll (T* obj, void (T::*TMethod) (ParamTypes...));

  void DisconnectOne (Event<ParamTypes...>& other);

  void DisconnectAll (Event<ParamTypes...>& other);

  void DisconnectAllSignals ();

  void DisconnectAllSlots ();

  virtual void Invoke (ParamTypes ... Args);

protected:

  virtual void AuditDestroyingSignal (Invoker* signal) final;

  void PushBackInvoker (PracticalbeInvoker<ParamTypes...>* signal);

  void PushFrontInvoker (PracticalbeInvoker<ParamTypes...>* signal);

  void InsertInvoker (int index, PracticalbeInvoker<ParamTypes...>* signal);

  void RemoveAllInvokers ();

private:

  Invoker* head_signal_;
  Invoker* tail_signal_;

  Invoker* iterator_;  // a pointer to iterate through all connections
  bool iterator_removed_;

};

template<typename ... ParamTypes>
inline Event<ParamTypes...>::Event ()
: AbstractTrackable(),
  head_signal_(0),
  tail_signal_(0),
  iterator_(0),
  iterator_removed_(false)
{
}

template<typename ... ParamTypes>
Event<ParamTypes...>::~Event ()
{
  RemoveAllInvokers();
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::Connect (T* obj, void (T::*method) (ParamTypes...))
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {

    Slot* downstream = new Slot;

    Delegate<void, ParamTypes...> d =
        Delegate<void, ParamTypes...>::template from_method<T>(obj, method);
    DelegateInvoker<ParamTypes...>* upstream = new DelegateInvoker<
        ParamTypes...>(d);

    Link(upstream, downstream);

    this->PushBackInvoker(upstream);
    add_slot(trackable_object, downstream);

    return;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Connect (Event<ParamTypes...>& other)
{
  EventInvoker<ParamTypes...>* upstream = new EventInvoker<ParamTypes...>(
      &other);
  Slot* downstream = new Slot;
  Link(upstream, downstream);
  this->PushBackInvoker(upstream);
  add_slot(&other, downstream);
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::DisconnectOne (T* obj, void (T::*method) (ParamTypes...))
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateInvoker<ParamTypes...>* conn = 0;
    for (Invoker* p = tail_signal_; p; p = p->previous) {
      conn = dynamic_cast<DelegateInvoker<ParamTypes...>*>(p);
      if (conn && (conn->delegate().template equal<T>(obj, method))) {
        delete conn;
        break;
      }
    }

    return;
  }
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::DisconnectAll (T* obj, void (T::*method) (ParamTypes...))
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateInvoker<ParamTypes...>* conn = 0;
    for (Invoker* p = tail_signal_; p; p = p->previous) {
      conn = dynamic_cast<DelegateInvoker<ParamTypes...>*>(p);
      if (conn && (conn->delegate().template equal<T>(obj, method)))
        delete conn;
    }

    return;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectOne (Event<ParamTypes...>& other)
{
  EventInvoker<ParamTypes...>* conn = 0;
  for (Invoker* p = tail_signal_; p; p = p->previous) {
    conn = dynamic_cast<EventInvoker<ParamTypes...>*>(p);
    if (conn && (conn->event() == (&other))) {
      delete conn;
      break;
    }
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAll (Event<ParamTypes...>& other)
{
  EventInvoker<ParamTypes...>* conn = 0;
  for (Invoker* p = tail_signal_; p; p = p->previous) {
    conn = dynamic_cast<EventInvoker<ParamTypes...>*>(p);
    if (conn && (conn->event() == (&other))) delete conn;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAllSignals()
{
  RemoveAllInvokers();
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAllSlots ()
{
  RemoveAllSlots();
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Invoke (ParamTypes ... Args)
{
  iterator_ = head_signal_;
  while (iterator_) {
    static_cast<PracticalbeInvoker<ParamTypes...>*>(iterator_)->Invoke(Args...);

    // check if iterator was deleted when being invoked
    // (iterator_removed_ was set via the virtual AuditDestroyingConnection()
    if (iterator_removed_)
      iterator_removed_ = false;
    else
      iterator_ = iterator_->next;
  }
  iterator_ = 0;
  iterator_removed_ = false;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::AuditDestroyingSignal (Invoker* signal)
{
  if (signal == head_signal_) {
    head_signal_ = signal->next;
  }

  if (signal == tail_signal_) {
    tail_signal_ = signal->previous;
  }

  if (signal == iterator_) {
    iterator_removed_ = true;
    iterator_ = iterator_->next;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::PushBackInvoker(PracticalbeInvoker<ParamTypes...>* signal)
{
#ifdef DEBUG
  assert(signal->trackable_object == 0);
#endif

  if (tail_signal_) {
    tail_signal_->next = signal;
    signal->previous = tail_signal_;
  } else {
#ifdef DEBUG
    assert(head_signal_ == 0);
#endif
    signal->previous = 0;
    head_signal_ = signal;
  }
  tail_signal_ = signal;
  signal->next = 0;
  signal->trackable_object = this;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::PushFrontInvoker(PracticalbeInvoker<ParamTypes...>* signal)
{
#ifdef DEBUG
  assert(node->trackable_object == 0);
#endif

  if (head_signal_) {
    head_signal_->previous = signal;
    signal->next = head_signal_;
  } else {
#ifdef DEBUG
    assert(tail_signal_ == 0);
#endif
    signal->next = 0;
    tail_signal_ = signal;
  }
  head_signal_ = signal;

  signal->previous = 0;
  signal->trackable_object = this;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::InsertInvoker(int index, PracticalbeInvoker<ParamTypes...>* signal)
{
#ifdef DEBUG
  assert(signal->trackable_object == 0);
#endif

  if (head_signal_ == 0) {
#ifdef DEBUG
    assert(tail_signal_ == 0);
#endif

    signal->next = 0;
    tail_signal_ = signal;
    head_signal_ = signal;
    signal->previous = 0;
  } else {
    if (index > 0) {

      Invoker* p = head_signal_;

      while (p && (index > 0)) {
        if (p->next == 0) break;
        p = p->next;
        index--;
      }

      if (index == 0) {  // insert
        signal->previous = p->previous;
        signal->next = p;
        p->previous->next = signal;
        p->previous = signal;
      } else {  // same as push back
#ifdef DEBUG
        assert(p == tail_signal_);
#endif
        tail_signal_->next = signal;
        signal->previous = tail_signal_;
        tail_signal_ = signal;
        signal->next = 0;
      }

    } else {  // same as push front
      head_signal_->previous = signal;
      signal->next = head_signal_;
      head_signal_ = signal;
      signal->previous = 0;
    }
  }
  signal->trackable_object = this;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::RemoveAllInvokers()
{
  Invoker* tmp = 0;
  Invoker* p = head_signal_;

  while (p) {
    tmp = p->next;
    delete p;
    p = tmp;
  }
}

} // namespace CppEvent

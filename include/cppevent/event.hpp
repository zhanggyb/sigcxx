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
#include <cppevent/delegate-signal.hpp>
#include <cppevent/event-signal.hpp>
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

  void DisconnectAllSignals ();

  void DisconnectAllSlots ();

  virtual void Invoke (ParamTypes ... Args);

protected:

  virtual void AuditDestroyingSignal (Signal* signal) final;

  void PushBackSignal (InvokableSignal<ParamTypes...>* signal);

  void PushFrontSignal (InvokableSignal<ParamTypes...>* signal);

  void InsertSignal (int index, InvokableSignal<ParamTypes...>* signal);

  void RemoveAllSignals ();

private:

  Signal* head_signal_;
  Signal* tail_signal_;

  Signal* iterator_;  // a pointer to iterate through all connections
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
  RemoveAllSignals();
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
    DelegateSignal<ParamTypes...>* upstream = new DelegateSignal<
        ParamTypes...>(d);

    Link(upstream, downstream);

    this->PushBackSignal(upstream);
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
    DelegateSignal<ParamTypes...>* upstream = new DelegateSignal<
        ParamTypes...>(d);

    Link(upstream, downstream);

    this->PushBackSignal(upstream);
    add_slot(trackable_object, downstream);

    return;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Connect (Event<ParamTypes...>& other)
{
  EventSignal<ParamTypes...>* upstream = new EventSignal<ParamTypes...>(
      &other);
  Slot* downstream = new Slot;
  Link(upstream, downstream);
  this->PushBackSignal(upstream);
  add_slot(&other, downstream);
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...)>
void Event<ParamTypes...>::DisconnectOne (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateSignal<ParamTypes...>* conn = 0;
    for (Signal* p = tail_signal_; p; p = p->previous) {
      conn = dynamic_cast<DelegateSignal<ParamTypes...>*>(p);
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
    DelegateSignal<ParamTypes...>* conn = 0;
    for (Signal* p = tail_signal_; p; p = p->previous) {
      conn = dynamic_cast<DelegateSignal<ParamTypes...>*>(p);
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
    DelegateSignal<ParamTypes...>* conn = 0;
    for (Signal* p = tail_signal_; p; p = p->previous) {
      conn = dynamic_cast<DelegateSignal<ParamTypes...>*>(p);
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
    DelegateSignal<ParamTypes...>* conn = 0;
    for (Signal* p = tail_signal_; p; p = p->previous) {
      conn = dynamic_cast<DelegateSignal<ParamTypes...>*>(p);
      if (conn && (conn->delegate().template equal<T, TMethod>(obj)))
        delete conn;
    }
    return;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectOne (Event<ParamTypes...>& other)
{
  EventSignal<ParamTypes...>* conn = 0;
  for (Signal* p = tail_signal_; p; p = p->previous) {
    conn = dynamic_cast<EventSignal<ParamTypes...>*>(p);
    if (conn && (conn->event() == (&other))) {
      delete conn;
      break;
    }
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAll (Event<ParamTypes...>& other)
{
  EventSignal<ParamTypes...>* conn = 0;
  for (Signal* p = tail_signal_; p; p = p->previous) {
    conn = dynamic_cast<EventSignal<ParamTypes...>*>(p);
    if (conn && (conn->event() == (&other))) delete conn;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAllSignals()
{
  RemoveAllSignals();
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
    static_cast<InvokableSignal<ParamTypes...>*>(iterator_)->Invoke(Args...);

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
void Event<ParamTypes...>::AuditDestroyingSignal (Signal* signal)
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
void Event<ParamTypes...>::PushBackSignal(InvokableSignal<ParamTypes...>* signal)
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
void Event<ParamTypes...>::PushFrontSignal(InvokableSignal<ParamTypes...>* signal)
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
void Event<ParamTypes...>::InsertSignal(int index, InvokableSignal<ParamTypes...>* signal)
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

      Signal* p = head_signal_;

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
void Event<ParamTypes...>::RemoveAllSignals()
{
  Signal* tmp = 0;
  Signal* p = head_signal_;

  while (p) {
    tmp = p->next;
    delete p;
    p = tmp;
  }
}

} // namespace CppEvent

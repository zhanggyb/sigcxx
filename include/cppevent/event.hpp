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
#include <cppevent/binding.hpp>
#include <cppevent/delegate-token.hpp>
#include <cppevent/event-token.hpp>

namespace CppEvent {

/**
 * @brief Abstract class for event
 */
class Trackable: public AbstractTrackable
{
 public:
  
  inline Trackable ()
      : AbstractTrackable()
  { }
  
  virtual ~Trackable ()
  { }
  
 protected:
  
  virtual void AuditDestroyingToken (Token* token) final
  { }
};

// ---------

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

  void DisconnectAllInvokers ();

  void DisconnectAllSlots ();

  virtual void Invoke (ParamTypes ... Args);

 protected:

  virtual void AuditDestroyingToken (Token* token) final;

  void PushBackToken (InvokableToken<ParamTypes...>* token);

  void PushFrontToken (InvokableToken<ParamTypes...>* token);

  void InsertToken (int index, InvokableToken<ParamTypes...>* token);

  void RemoveAllTokens ();

 private:

  Token* first_token_;
  Token* last_token_;

  Token* iterator_;  // a pointer to iterate through all connections
  bool iterator_removed_;

};

template<typename ... ParamTypes>
inline Event<ParamTypes...>::Event ()
    : AbstractTrackable(),
      first_token_(0),
      last_token_(0),
      iterator_(0),
      iterator_removed_(false)
{
}

template<typename ... ParamTypes>
Event<ParamTypes...>::~Event ()
{
  RemoveAllTokens();
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::Connect (T* obj, void (T::*method) (ParamTypes...))
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {

    Binding* downstream = new Binding;

    Delegate<void, ParamTypes...> d =
        Delegate<void, ParamTypes...>::template from_method<T>(obj, method);
    DelegateToken<ParamTypes...>* upstream = new DelegateToken<
      ParamTypes...>(d);

    Link(upstream, downstream);

    this->PushBackToken(upstream);
    add_binding(trackable_object, downstream);

    return;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Connect (Event<ParamTypes...>& other)
{
  EventToken<ParamTypes...>* upstream = new EventToken<ParamTypes...>(
      &other);
  Binding* downstream = new Binding;
  Link(upstream, downstream);
  this->PushBackToken(upstream);
  add_binding(&other, downstream);
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::DisconnectOne (T* obj, void (T::*method) (ParamTypes...))
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateToken<ParamTypes...>* conn = 0;
    for (Token* p = last_token_; p; p = p->previous) {
      conn = dynamic_cast<DelegateToken<ParamTypes...>*>(p);
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
    DelegateToken<ParamTypes...>* conn = 0;
    for (Token* p = last_token_; p; p = p->previous) {
      conn = dynamic_cast<DelegateToken<ParamTypes...>*>(p);
      if (conn && (conn->delegate().template equal<T>(obj, method)))
        delete conn;
    }

    return;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectOne (Event<ParamTypes...>& other)
{
  EventToken<ParamTypes...>* conn = 0;
  for (Token* p = last_token_; p; p = p->previous) {
    conn = dynamic_cast<EventToken<ParamTypes...>*>(p);
    if (conn && (conn->event() == (&other))) {
      delete conn;
      break;
    }
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAll (Event<ParamTypes...>& other)
{
  EventToken<ParamTypes...>* conn = 0;
  for (Token* p = last_token_; p; p = p->previous) {
    conn = dynamic_cast<EventToken<ParamTypes...>*>(p);
    if (conn && (conn->event() == (&other))) delete conn;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAllInvokers()
{
  RemoveAllTokens();
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAllSlots ()
{
  RemoveAllBindings();
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Invoke (ParamTypes ... Args)
{
  iterator_ = first_token_;
  while (iterator_) {
    static_cast<InvokableToken<ParamTypes...>*>(iterator_)->Invoke(Args...);

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
void Event<ParamTypes...>::AuditDestroyingToken (Token* token)
{
  if (token == first_token_) {
    first_token_ = token->next;
  }

  if (token == last_token_) {
    last_token_ = token->previous;
  }

  if (token == iterator_) {
    iterator_removed_ = true;
    iterator_ = iterator_->next;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::PushBackToken(InvokableToken<ParamTypes...>* token)
{
#ifdef DEBUG
  assert(token->trackable_object == 0);
#endif

  if (last_token_) {
    last_token_->next = token;
    token->previous = last_token_;
  } else {
#ifdef DEBUG
    assert(first_token_ == 0);
#endif
    token->previous = 0;
    first_token_ = token;
  }
  last_token_ = token;
  token->next = 0;
  token->trackable_object = this;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::PushFrontToken(InvokableToken<ParamTypes...>* token)
{
#ifdef DEBUG
  assert(node->trackable_object == 0);
#endif

  if (first_token_) {
    first_token_->previous = token;
    token->next = first_token_;
  } else {
#ifdef DEBUG
    assert(last_token_ == 0);
#endif
    token->next = 0;
    last_token_ = token;
  }
  first_token_ = token;

  token->previous = 0;
  token->trackable_object = this;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::InsertToken(int index, InvokableToken<ParamTypes...>* token)
{
#ifdef DEBUG
  assert(token->trackable_object == 0);
#endif

  if (first_token_ == 0) {
#ifdef DEBUG
    assert(last_token_ == 0);
#endif

    token->next = 0;
    last_token_ = token;
    first_token_ = token;
    token->previous = 0;
  } else {
    if (index > 0) {

      Token* p = first_token_;

      while (p && (index > 0)) {
        if (p->next == 0) break;
        p = p->next;
        index--;
      }

      if (index == 0) {  // insert
        token->previous = p->previous;
        token->next = p;
        p->previous->next = token;
        p->previous = token;
      } else {  // same as push back
#ifdef DEBUG
        assert(p == last_token_);
#endif
        last_token_->next = token;
        token->previous = last_token_;
        last_token_ = token;
        token->next = 0;
      }

    } else {  // same as push front
      first_token_->previous = token;
      token->next = first_token_;
      first_token_ = token;
      token->previous = 0;
    }
  }
  token->trackable_object = this;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::RemoveAllTokens()
{
  Token* tmp = 0;
  Token* p = first_token_;

  while (p) {
    tmp = p->next;
    delete p;
    p = tmp;
  }
}

} // namespace CppEvent

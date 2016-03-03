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

#include "private/abstract-trackable.hpp"
#include "private/delegate-token.hpp"
#include "private/event-token.hpp"

#include <thread>

namespace CppEvent {

enum ConnectOption {
  ConnectFirst = 0,
  ConnectLast = 1
};

enum DisconnectOption {
  DisconnectFirst = 0,
  DisconnectLast = 1,
  DisconnectAll = 2
};

/**
 * @brief The base class for observer
 */
class Observer: public AbstractTrackable
{
 public:

  inline Observer ()
      : AbstractTrackable()
  { }

  virtual ~Observer ()
  { }

 protected:

  virtual void AuditDestroyingToken (Token* token) final
  { }
};

// Event declaration:

template<typename ... ParamTypes>
class Event: public AbstractTrackable
{
 public:

  inline Event ();

  virtual ~Event ();

  /**
   * @brief Connect this event to a method in a observer
   */
  template<typename T>
  void Connect (T* obj, void (T::*method) (ParamTypes...), ConnectOption opt = ConnectLast);

  void Connect (Event<ParamTypes...>& other, ConnectOption opt = ConnectLast);

  /**
   * @brief Disconnect the last delegate to a method
   */
  template<typename T>
  void Disconnect (T* obj, void (T::*method) (ParamTypes...), DisconnectOption opt = DisconnectLast);

  /**
   * @brief Disconnect the last event
   */
  void Disconnect (Event<ParamTypes...>& other, DisconnectOption opt = DisconnectLast);

  void RemoveAllOutConnections ();

  template<typename T>
  bool IsConnected (T* obj, void (T::*method) (ParamTypes...)) const;

  bool IsConnected (Event<ParamTypes...>& other) const;

  template<typename T>
  std::size_t CountConnections (T* obj, void (T::*method) (ParamTypes...)) const;

  std::size_t CountConnections (Event<ParamTypes...>& other) const;

  std::size_t CountOutConnections () const;

  void Fire (ParamTypes ... Args);

 protected:

  virtual void AuditDestroyingToken (Token* token) final;

  void PushBackToken (InvokableToken<ParamTypes...>* token);

  void PushFrontToken (InvokableToken<ParamTypes...>* token);

  void InsertToken (int index, InvokableToken<ParamTypes...>* token);

 private:

  static inline void set_bit (int& v, int m) { v |= m;}
  static inline void clear_bit (int& v, int m) {v &= ~m;}

  enum EventFlagMask {
    EventIteratorRemoveMask    = 0x01,	// if iterator removed: 1 - true, 0 - false
    EventIteratorDirectionMask = 0x01 << 1 // direction when iterating: 0 - forward, 1 - backward
  };

  Token* first_token_;
  Token* last_token_;

  Token* iterator_;  // a pointer to iterate through all connections
  int flag_;
};

// EventRef declaration:

template<typename ... ParamTypes>
class EventRef
{
 public:

  EventRef () = delete;

  inline EventRef(Event<ParamTypes...>& event)
      : event_(&event)
  { }

  inline EventRef(const EventRef<ParamTypes...>& orig)
      : event_(orig.event_)
  { }

  ~EventRef() {}

  inline EventRef<ParamTypes...>& operator = (const EventRef<ParamTypes...>& orig) {
    event_ = orig.event_;
    return *this;
  }

  template<typename T>
  inline void connect (T* obj, void (T::*method)(ParamTypes...), ConnectOption opt = ConnectLast) {
    event_->Connect(obj, method, opt);
  }

  template<typename T>
  inline void disconnect (T* obj, void (T::*method)(ParamTypes...), DisconnectOption opt = DisconnectLast) {
    event_->Disconnect(obj, method, opt);
  }

  inline void connect (Event<ParamTypes...>& event, ConnectOption opt = ConnectLast) {
    event_->Connect(event, opt);
  }

  inline void disconnect (Event<ParamTypes...>& event, DisconnectOption opt = DisconnectLast) {
    event_->Disconnect(event, opt);
  }

  inline void connect (const EventRef<ParamTypes...>& other, ConnectOption opt = ConnectLast) {
    event_->Connect(*other.event_, opt);
  }

  inline void disconnect (const EventRef<ParamTypes...>& other, DisconnectOption opt = DisconnectLast) {
    event_->Disconnect(*other.event_, opt);
  }

  inline void remove_all_out_connections () {
    event_->RemoveAllOutConnections();
  }

  inline void remove_all_in_connections () {
    event_->RemoveAllInConnections();
  }

  template<typename T>
  inline bool is_connected (T* obj, void (T::*method) (ParamTypes...)) const {
    return event_->IsConnected(obj, method);
  }

  inline bool is_connected (Event<ParamTypes...>& other) const {
    return event_->IsConnected(other);
  }

  template<typename T>
  inline std::size_t count_connections (T* obj, void (T::*method) (ParamTypes...)) const {
    return event_->CountConnections(obj, method);
  }

  inline std::size_t count_connections (Event<ParamTypes...>& other) const {
    return event_->CountConnections(other);
  }

  inline std::size_t count_out_connections () const {
    return event_->CountOutConnections();
  }

 private:

  Event<ParamTypes...>* event_;
};

// Event implementation:

template<typename ... ParamTypes>
inline Event<ParamTypes...>::Event ()
    : AbstractTrackable(),
      first_token_(0),
      last_token_(0),
      iterator_(0),
      flag_(0)
{
}

template<typename ... ParamTypes>
Event<ParamTypes...>::~Event ()
{
  RemoveAllOutConnections();
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::Connect (T* obj, void (T::*method) (ParamTypes...), ConnectOption opt)
{
  Binding* downstream = new Binding;
  Delegate<void, ParamTypes...> d =
      Delegate<void, ParamTypes...>::template from_method<T>(obj, method);
  DelegateToken<ParamTypes...>* upstream = new DelegateToken<
    ParamTypes...>(d);

  link(upstream, downstream);
  switch (opt) {
    case ConnectFirst:
      this->PushFrontToken(upstream);
      push_front(obj, downstream);
      break;
    default:
      this->PushBackToken(upstream);
      push_back(obj, downstream);
      break;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Connect (Event<ParamTypes...>& other, ConnectOption opt)
{
  EventToken<ParamTypes...>* upstream = new EventToken<ParamTypes...>(
      other);
  Binding* downstream = new Binding;

  link(upstream, downstream);
  switch (opt) {
    case ConnectFirst:
      this->PushFrontToken(upstream);
      push_front(&other, downstream);
      break;
    default:
      this->PushBackToken(upstream);
      push_back(&other, downstream);
      break;
  }
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::Disconnect (T* obj, void (T::*method) (ParamTypes...), DisconnectOption opt)
{
  DelegateToken<ParamTypes...>* conn = 0;

  switch(opt) {
    case DisconnectFirst: {
      for (Token* p = first_token_; p; p = p->next) {
        conn = dynamic_cast<DelegateToken<ParamTypes...>*>(p);
        if (conn && (conn->delegate().template equal<T>(obj, method))) {
          delete conn;
          break;
        }
      }
      break;
    }
    case DisconnectLast: {
      for (Token* p = last_token_; p; p = p->previous) {
        conn = dynamic_cast<DelegateToken<ParamTypes...>*>(p);
        if (conn && (conn->delegate().template equal<T>(obj, method))) {
          delete conn;
          break;
        }
      }
      break;
    }
    default: {
      Token* p1 = 0;
      Token* p2 = 0;

      if(flag_ & EventIteratorDirectionMask) {
        p1 = last_token_;
        while(p1) {
          p2 = p1->previous;
          conn = dynamic_cast<DelegateToken<ParamTypes...>*>(p1);
          if (conn && (conn->delegate().template equal<T>(obj, method))) {
            delete conn;
          }
          p1 = p2;
        }
      } else {
        p1 = first_token_;
        while(p1) {
          p2 = p1->next;
          conn = dynamic_cast<DelegateToken<ParamTypes...>*>(p1);
          if (conn && (conn->delegate().template equal<T>(obj, method))) {
            delete conn;
          }
          p1 = p2;
        }
      }
      break;
    }
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Disconnect (Event<ParamTypes...>& other, DisconnectOption opt)
{
  EventToken<ParamTypes...>* conn = 0;

  switch(opt) {
    case DisconnectFirst: {
      for (Token* p = first_token_; p; p = p->next) {
        conn = dynamic_cast<EventToken<ParamTypes...>*>(p);
        if (conn && (conn->event() == (&other))) {
          delete conn;
          break;
        }
      }
      break;
    }
    case DisconnectLast: {
      for (Token* p = last_token_; p; p = p->previous) {
        conn = dynamic_cast<EventToken<ParamTypes...>*>(p);
        if (conn && (conn->event() == (&other))) {
          delete conn;
          break;
        }
      }
      break;
    }
    default: {
      Token* p1 = 0;
      Token* p2 = 0;

      if(flag_ & EventIteratorDirectionMask) {
        p1 = last_token_;
        while(p1) {
          p2 = p1->previous;
          conn = dynamic_cast<EventToken<ParamTypes...>*>(p1);
          if (conn && (conn->event() == (&other))) {
            delete conn;
          }
          p1 = p2;
        }
      } else {
        p1 = first_token_;
        while(p1) {
          p2 = p1->next;
          conn = dynamic_cast<EventToken<ParamTypes...>*>(p1);
          if (conn && (conn->event() == (&other))) {
            delete conn;
          }
          p1 = p2;
        }
      }
      break;
    }
  }
}

template<typename ... ParamTypes>
template<typename T>
bool Event<ParamTypes...>::IsConnected (T* obj, void (T::*method) (ParamTypes...)) const
{
  DelegateToken<ParamTypes...>* conn = 0;

  for(Token* p = first_token_; p; p = p->next) {
    conn = dynamic_cast<DelegateToken<ParamTypes...>*>(p);
    if (conn && (conn->delegate().template equal<T>(obj, method))) {
      return true;
    }
  }
  return false;
}

template<typename ... ParamTypes>
bool Event<ParamTypes...>::IsConnected (Event<ParamTypes...>& other) const
{
  EventToken<ParamTypes...>* conn = 0;

  for(Token* p = first_token_; p; p = p->next) {
    conn = dynamic_cast<EventToken<ParamTypes...>*>(p);
    if (conn && (conn->event() == (&other))) {
      return true;
    }
  }
  return false;
}

template<typename ... ParamTypes>
template<typename T>
std::size_t Event<ParamTypes...>::CountConnections (T* obj, void (T::*method) (ParamTypes...)) const
{
  std::size_t count = 0;
  DelegateToken<ParamTypes...>* conn = 0;

  for(Token* p = first_token_; p; p = p->next) {
    conn = dynamic_cast<DelegateToken<ParamTypes...>*>(p);
    if (conn && (conn->delegate().template equal<T>(obj, method))) {
      count++;
    }
  }
  return count;
}

template<typename ... ParamTypes>
std::size_t Event<ParamTypes...>::CountConnections (Event<ParamTypes...>& other) const
{
  std::size_t count = 0;
  EventToken<ParamTypes...>* conn = 0;

  for(Token* p = first_token_; p; p = p->next) {
    conn = dynamic_cast<EventToken<ParamTypes...>*>(p);
    if (conn && (conn->event() == (&other))) {
      count++;
    }
  }
  return count;
}

template<typename ... ParamTypes>
std::size_t Event<ParamTypes...>::CountOutConnections () const
{
  std::size_t count = 0;
  for(Token* p = first_token_; p; p = p->next) {
    count++;
  }
  return count;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Fire (ParamTypes ... Args)
{
  clear_bit(flag_, EventIteratorDirectionMask);
  iterator_ = first_token_;
  while (iterator_) {
    static_cast<InvokableToken<ParamTypes...>*>(iterator_)->Invoke(Args...);

    // check if iterator was deleted when being invoked
    // (EventIteratorRemoveMask was set via the virtual AuditDestroyingConnection()
    if (flag_ & EventIteratorRemoveMask)
      clear_bit(flag_, EventIteratorRemoveMask);
    else
      iterator_ = iterator_->next;
  }
  iterator_ = 0;
  clear_bit(flag_, EventIteratorRemoveMask);
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::AuditDestroyingToken (Token* token)
{
  if (token == first_token_) first_token_ = token->next;
  if (token == last_token_) last_token_ = token->previous;
  if (token == iterator_) {
    if(flag_ & EventIteratorDirectionMask)
      iterator_ = iterator_->previous;
    else
      iterator_ = iterator_->next;

    set_bit(flag_, EventIteratorRemoveMask);
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
  assert(token->trackable_object == 0);
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
void Event<ParamTypes...>::RemoveAllOutConnections()
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

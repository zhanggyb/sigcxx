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

#ifdef DEBUG
#include <cassert>
#endif
#include <cstddef>

#include "delegate.hpp"

namespace CppEvent {

inline const char* string_version () {
  return "0.9";
}

inline int major_version () {
  return 0;
}

inline int minor_version () {
  return 9;
}

// forward declaration
class AbstractTrackable;
template<typename ... ParamTypes> class Event;

namespace details {

struct Token;
    
/**
 * @brief The event binding
 */
struct Binding
{
  inline Binding ()
      : trackable_object(0),
        previous(0),
        next(0),
        token(0)
  {
  }
      
  ~Binding ();
      
  AbstractTrackable* trackable_object;
  Binding* previous;
  Binding* next;
  Token* token;
};
    
/**
 * @brief The event source
 */
struct Token
{
  inline Token ()
      : trackable_object(0),
        previous(0),
        next(0),
        binding(0)
  {
  }
      
  virtual ~Token ();
      
  AbstractTrackable* trackable_object;
  Token* previous;
  Token* next;
  Binding* binding;
      
};

template<typename ... ParamTypes>
class InvokableToken: public Token
{
 public:
      
  inline InvokableToken ();
      
  virtual ~InvokableToken ();
      
  virtual void Invoke (ParamTypes ... Args);
};
    
template<typename ... ParamTypes>
inline InvokableToken<ParamTypes...>::InvokableToken ()
    : Token()
{
}
    
template<typename ... ParamTypes>
InvokableToken<ParamTypes...>::~InvokableToken ()
{
}
    
template<typename ... ParamTypes>
void InvokableToken<ParamTypes...>::Invoke (ParamTypes ... Args)
{
  // Override this in sub class
}
    
template<typename ... ParamTypes>
class DelegateToken : public InvokableToken < ParamTypes... >
{
 public:
      
  DelegateToken() = delete;
      
  inline DelegateToken(const Delegate<void, ParamTypes...>& d);
      
  virtual ~DelegateToken();
      
  virtual void Invoke(ParamTypes... Args) override;
      
  const Delegate<void, ParamTypes...>& delegate () const
  {
    return delegate_;
  }
      
 private:
      
  Delegate<void, ParamTypes...> delegate_;
      
};
    
template<typename ... ParamTypes>
inline DelegateToken<ParamTypes...>::DelegateToken(const Delegate<void, ParamTypes...>& d)
    : InvokableToken<ParamTypes...>(), delegate_(d)
{
}
    
template<typename ... ParamTypes>
DelegateToken<ParamTypes...>::~DelegateToken()
{
}
    
template<typename ... ParamTypes>
void DelegateToken<ParamTypes...>::Invoke(ParamTypes... Args)
{
  delegate_(Args...);
}

    
template<typename ... ParamTypes>
class EventToken : public InvokableToken < ParamTypes... >
{
 public:
      
  EventToken () = delete;
      
  inline EventToken(Event<ParamTypes...>& event);
      
  virtual ~EventToken();
      
  virtual void Invoke(ParamTypes... Args) override;
      
  inline const Event<ParamTypes...>* event () const;
      
 private:
      
  Event<ParamTypes...>* event_;
};
    
template<typename ... ParamTypes>
inline EventToken<ParamTypes...>::EventToken (Event<ParamTypes...>& event)
    : InvokableToken<ParamTypes...>(), event_(&event)
{
}
    
template<typename ... ParamTypes>
EventToken<ParamTypes...>::~EventToken()
{
}
    
template<typename ... ParamTypes>
void EventToken<ParamTypes...>::Invoke(ParamTypes... Args)
{
  event_->Fire(Args...);
}
    
template<typename ... ParamTypes>
inline const Event<ParamTypes...>* EventToken<ParamTypes...>::event() const
{
  return event_;
}

}  // namespace details
  
/**
 * @brief Abstract class for event
 */
class AbstractTrackable
{
  friend struct details::Binding;
  friend struct details::Token;
    
 public:
    
  inline AbstractTrackable ()
      : first_binding_(0), last_binding_(0)
  {
  }
    
  virtual ~AbstractTrackable ();
    
  void RemoveAllInConnections ();
    
  std::size_t CountInConnections () const;
    
 protected:
    
  virtual void AuditDestroyingToken (details::Token* token) = 0;
    
  void PushBackBinding (details::Binding* binding);
    
  void PushFrontBinding (details::Binding* binding);
    
  void InsertBinding (int index, details::Binding* binding);
    
  static inline void link (details::Token* token, details::Binding* binding)
  {
#ifdef DEBUG
    assert((token->binding == 0) && (binding->token == 0));
#endif
      
    token->binding = binding;
    binding->token = token;
  }
    
  static inline void push_front (AbstractTrackable* trackable,
                                 details::Binding* conn)
  {
    trackable->PushBackBinding(conn);
  }
    
  static inline void push_back (AbstractTrackable* trackable,
                                details::Binding* conn)
  {
    trackable->PushBackBinding(conn);
  }
    
  static inline void insert (AbstractTrackable* trackable,
                             details::Binding* conn,
                             int index = 0)
  {
    trackable->InsertBinding(index, conn);
  }
    
  inline details::Binding* first_binding () const
  {
    return first_binding_;
  }
    
  inline details::Binding* last_binding () const
  {
    return last_binding_;
  }
    
 private:
    
  details::Binding* first_binding_;
  details::Binding* last_binding_;
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

  inline Observer(const Observer& orig)
  { }
  
  inline Observer& operator = (const Observer& orig)
  {
    return *this;
  }
  
 protected:

  virtual void AuditDestroyingToken (details::Token* token) final
  { }
};

// Event declaration:

template<typename ... ParamTypes>
class Event: public AbstractTrackable
{
 public:

  inline Event ();

  virtual ~Event ();

  Event (const Event& orig) = delete;
  
  Event& operator = (const Event& orig) = delete;
  
  /**
   * @brief Connect this event to a method in a observer
   */
  template<typename T>
  void Connect (T* obj, void (T::*method) (ParamTypes...), int index = -1);

  void Connect (Event<ParamTypes...>& other, int index = -1);

  /**
   * @brief Disconnect all delegates to a method
   */
  template<typename T>
  void DisconnectAll (T* obj, void (T::*method) (ParamTypes...));

  /**
   * @brief Disconnect all events
   */
  void DisconnectAll (Event<ParamTypes...>& other);

  /**
   * @brief Disconnect the last delegate to a method
   */
  template<typename T>
  void DisconnectOnce (T* obj, void (T::*method) (ParamTypes...), int start_pos = -1);

  /**
   * @brief Disconnect the last event
   */
  void DisconnectOnce (Event<ParamTypes...>& other, int start_pos = -1);

  void RemoveAllOutConnections ();

  template<typename T>
  bool IsConnected (T* obj, void (T::*method) (ParamTypes...)) const;

  bool IsConnected (Event<ParamTypes...>& other) const;

  template<typename T>
  std::size_t CountConnections (T* obj, void (T::*method) (ParamTypes...)) const;

  std::size_t CountConnections (Event<ParamTypes...>& other) const;

  std::size_t CountOutConnections () const;

  void Fire (ParamTypes ... Args);
  
  inline void operator () (ParamTypes ... Args);

 protected:

  virtual void AuditDestroyingToken (details::Token* token) final;

  void PushBackToken (details::InvokableToken<ParamTypes...>* token);

  void PushFrontToken (details::InvokableToken<ParamTypes...>* token);

  void InsertToken (int index, details::InvokableToken<ParamTypes...>* token);

  inline details::Token* first_token () const
  {
    return first_token_;
  }
  
  inline details::Token* last_token () const
  {
    return last_token_;
  }
  
 private:

  static inline void set_bit (int& v, int m) { v |= m;}
  static inline void clear_bit (int& v, int m) {v &= ~m;}

  enum EventFlagMask {
    EventIteratorRemoveMask    = 0x01,	// if iterator removed: 1 - true, 0 - false
    EventIteratorDirectionMask = 0x01 << 1 // direction when iterating: 0 - forward, 1 - backward
  };

  details::Token* first_token_;
  details::Token* last_token_;

  details::Token* iterator_;  // a pointer to iterate through all connections
  int flag_;
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
void Event<ParamTypes...>::Connect (T* obj, void (T::*method) (ParamTypes...), int index)
{
  details::Binding* downstream = new details::Binding;
  Delegate<void, ParamTypes...> d =
      Delegate<void, ParamTypes...>::template from_method<T>(obj, method);
  details::DelegateToken<ParamTypes...>* upstream = new details::DelegateToken<
    ParamTypes...>(d);

  link(upstream, downstream);
  InsertToken(index, upstream);
  push_back(obj, downstream);  // always push back binding, don't care about the position in observer
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Connect (Event<ParamTypes...>& other, int index)
{
  details::EventToken<ParamTypes...>* upstream = new details::EventToken<ParamTypes...>(
      other);
  details::Binding* downstream = new details::Binding;

  link(upstream, downstream);
  InsertToken(index, upstream);
  push_back(&other, downstream);  // always push back binding, don't care about the position in observer
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::DisconnectAll (T* obj, void (T::*method) (ParamTypes...))
{
  details::DelegateToken<ParamTypes...>* conn = 0;

  details::Token* p1 = 0;
  details::Token* p2 = 0;
  
  if(flag_ & EventIteratorDirectionMask) {
    p1 = last_token_;
    while(p1) {
      p2 = p1->previous;
      conn = dynamic_cast<details::DelegateToken<ParamTypes...>*>(p1);
      if (conn && (conn->delegate().template equal<T>(obj, method))) {
        delete conn;
      }
      p1 = p2;
    }
  } else {
    p1 = first_token_;
    while(p1) {
      p2 = p1->next;
      conn = dynamic_cast<details::DelegateToken<ParamTypes...>*>(p1);
      if (conn && (conn->delegate().template equal<T>(obj, method))) {
        delete conn;
      }
      p1 = p2;
    }
  }

}
  
template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAll (Event<ParamTypes...>& other)
{
  details::EventToken<ParamTypes...>* conn = 0;

  details::Token* p1 = 0;
  details::Token* p2 = 0;
  
  if(flag_ & EventIteratorDirectionMask) {
    p1 = last_token_;
    while(p1) {
      p2 = p1->previous;
      conn = dynamic_cast<details::EventToken<ParamTypes...>*>(p1);
      if (conn && (conn->event() == (&other))) {
        delete conn;
      }
      p1 = p2;
    }
  } else {
    p1 = first_token_;
    while(p1) {
      p2 = p1->next;
      conn = dynamic_cast<details::EventToken<ParamTypes...>*>(p1);
      if (conn && (conn->event() == (&other))) {
        delete conn;
      }
      p1 = p2;
    }
  }

}
  
template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::DisconnectOnce (T* obj, void (T::*method) (ParamTypes...), int start_pos)
{
  details::DelegateToken<ParamTypes...>* conn = 0;
  details::Token* start = 0;

  if (start_pos >= 0) {
    start = first_token_;
    
    while (start && (start_pos > 0)) {
      start = start->next;
      start_pos--;
    }
    
    if (start) {
      for (details::Token* p = start; p; p = p->next) {
        conn = dynamic_cast<details::DelegateToken<ParamTypes...>*>(p);
        if (conn && (conn->delegate().template equal<T>(obj, method))) {
          delete conn;
          break;
        }
      }
    }
  } else {
    start = last_token_;
    
    while (start && (start_pos < -1)) {
      start = start->previous;
      start_pos++;
    }
    
    if (start) {
      for (details::Token* p = last_token_; p; p = p->previous) {
        conn = dynamic_cast<details::DelegateToken<ParamTypes...>*>(p);
        if (conn && (conn->delegate().template equal<T>(obj, method))) {
          delete conn;
          break;
        }
      }
    }
    
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectOnce (Event<ParamTypes...>& other, int start_pos)
{
  details::EventToken<ParamTypes...>* conn = 0;
  details::Token* start = 0;
  
  if (start_pos >= 0) {
    start = first_token_;
    
    while (start && (start_pos > 0)) {
      start = start->next;
      start_pos--;
    }
    
    if (start) {
      for (details::Token* p = start; p; p = p->next) {
        conn = dynamic_cast<details::EventToken<ParamTypes...>*>(p);
        if (conn && (conn->event() == (&other))) {
          delete conn;
          break;
        }
      }
    }
  } else {
    start = last_token_;
    
    while (start && (start_pos < -1)) {
      start = start->previous;
      start_pos++;
    }
    
    if (start) {
      for (details::Token* p = last_token_; p; p = p->previous) {
        conn = dynamic_cast<details::EventToken<ParamTypes...>*>(p);
        if (conn && (conn->event() == (&other))) {
          delete conn;
          break;
        }
      }
    }
    
  }
}

template<typename ... ParamTypes>
template<typename T>
bool Event<ParamTypes...>::IsConnected (T* obj, void (T::*method) (ParamTypes...)) const
{
  details::DelegateToken<ParamTypes...>* conn = 0;

  for(details::Token* p = first_token_; p; p = p->next) {
    conn = dynamic_cast<details::DelegateToken<ParamTypes...>*>(p);
    if (conn && (conn->delegate().template equal<T>(obj, method))) {
      return true;
    }
  }
  return false;
}

template<typename ... ParamTypes>
bool Event<ParamTypes...>::IsConnected (Event<ParamTypes...>& other) const
{
  details::EventToken<ParamTypes...>* conn = 0;

  for(details::Token* p = first_token_; p; p = p->next) {
    conn = dynamic_cast<details::EventToken<ParamTypes...>*>(p);
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
  details::DelegateToken<ParamTypes...>* conn = 0;

  for(details::Token* p = first_token_; p; p = p->next) {
    conn = dynamic_cast<details::DelegateToken<ParamTypes...>*>(p);
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
  details::EventToken<ParamTypes...>* conn = 0;

  for(details::Token* p = first_token_; p; p = p->next) {
    conn = dynamic_cast<details::EventToken<ParamTypes...>*>(p);
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
  for(details::Token* p = first_token_; p; p = p->next) {
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
    static_cast<details::InvokableToken<ParamTypes...>*>(iterator_)->Invoke(Args...);

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
inline void Event<ParamTypes...>::operator()(ParamTypes ... Args)
{
  Fire(Args...);
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::AuditDestroyingToken (details::Token* token)
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
void Event<ParamTypes...>::PushBackToken(details::InvokableToken<ParamTypes...>* token)
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
void Event<ParamTypes...>::PushFrontToken(details::InvokableToken<ParamTypes...>* token)
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
void Event<ParamTypes...>::InsertToken(int index, details::InvokableToken<ParamTypes...>* token)
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
    if (index >= 0) {
      
      details::Token* p = first_token_;
#ifdef DEBUG
      assert(p != 0);
#endif
      
      while (p && (index > 0)) {
        p = p->next;
        index--;
      }
      
      if (p) {  // insert before p
        
        token->previous = p->previous;
        token->next = p;
        
        if (p->previous) p->previous->next = token;
        else first_token_ = token;
        
        p->previous = token;
        
      } else {  // push back
        
        last_token_->next = token;
        token->previous = last_token_;
        last_token_ = token;
        token->next = 0;
        
      }
      
    } else {
      
      details::Token* p = last_token_;
#ifdef DEBUG
      assert(p != 0);
#endif
      
      while (p && (index < -1)) {
        p = p->previous;
        index++;
      }
      
      if (p) {  // insert after p
        
        token->next = p->next;
        token->previous = p;
        
        if (p->next) p->next->previous = token;
        else last_token_ = token;
        
        p->next = token;
        
      } else {  // push front
        
        first_token_->previous = token;
        token->next = first_token_;
        first_token_ = token;
        token->previous = 0;
        
      }
      
    }
  }
  token->trackable_object = this;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::RemoveAllOutConnections()
{
  details::Token* tmp = 0;
  details::Token* p = first_token_;

  while (p) {
    tmp = p->next;
    delete p;
    p = tmp;
  }
}

} // namespace CppEvent

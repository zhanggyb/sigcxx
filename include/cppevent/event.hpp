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

// forward declaration
class Trackable;
template<typename ... ParamTypes> class Event;

namespace details {
    
struct Token;
    
/**
 * @brief The event binding
 */
struct Binding
{
  inline Binding ()
      : trackable_object(nullptr),
        previous(nullptr),
        next(nullptr),
        token(nullptr)
  {
  }
      
  ~Binding ();
      
  Trackable* trackable_object;
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
      : trackable_object(nullptr),
        previous(nullptr),
        next(nullptr),
        binding(nullptr)
  {
  }
      
  virtual ~Token ();
      
  Trackable* trackable_object;
  Token* previous;
  Token* next;
  Binding* binding;
      
};
    
    
}

class Meta
{
  template<typename ... ParamTypes> friend class Event;
    
 public:
    
  inline Meta ()
      : iterator_(nullptr)
  {
  }
    
  ~Meta();
    
 private:
  details::Token* iterator_;
};
  
namespace details {

template<typename ... ParamTypes>
class InvokableToken: public Token
{
 public:
      
  inline InvokableToken ();
      
  virtual ~InvokableToken ();
      
  virtual void Invoke (const Meta* meta_ptr, ParamTypes ... Args);
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
void InvokableToken<ParamTypes...>::Invoke (const Meta* meta_ptr, ParamTypes ... Args)
{
  // Override this in sub class
}
    
template<typename ... ParamTypes>
class DelegateToken : public InvokableToken < ParamTypes... >
{
 public:
      
  DelegateToken() = delete;
      
  inline DelegateToken(const Delegate<void, const Meta*, ParamTypes...>& d);
      
  virtual ~DelegateToken();
      
  virtual void Invoke(const Meta* meta_ptr, ParamTypes... Args) override;
      
  const Delegate<void, ParamTypes...>& delegate () const
  {
    return delegate_;
  }
      
 private:
      
  Delegate<void, const Meta*,ParamTypes...> delegate_;
      
};
    
template<typename ... ParamTypes>
inline DelegateToken<ParamTypes...>::DelegateToken(const Delegate<void, const Meta*, ParamTypes...>& d)
    : InvokableToken<ParamTypes...>(), delegate_(d)
{
}
    
template<typename ... ParamTypes>
DelegateToken<ParamTypes...>::~DelegateToken()
{
}
    
template<typename ... ParamTypes>
void DelegateToken<ParamTypes...>::Invoke(const Meta* meta_ptr, ParamTypes... Args)
{
  delegate_(meta_ptr, Args...);
}

    
template<typename ... ParamTypes>
class EventToken : public InvokableToken < ParamTypes... >
{
 public:
      
  EventToken () = delete;
      
  inline EventToken(Event<ParamTypes...>& event);
      
  virtual ~EventToken();
      
  virtual void Invoke(const Meta* meta_ptr, ParamTypes... Args) override;
      
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
void EventToken<ParamTypes...>::Invoke(const Meta* meta_ptr, ParamTypes... Args)
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
class Trackable
{
  friend struct details::Binding;
  friend struct details::Token;
    
 public:
    
  inline Trackable ()
      : first_binding_(nullptr), last_binding_(nullptr)
  {
  }
    
  virtual ~Trackable ();

  inline Trackable(const Trackable& orig)
  { }
  
  inline Trackable& operator = (const Trackable& orig)
  {
    return *this;
  }
  
  void RemoveAllInConnections ();
    
  std::size_t CountInConnections () const;
    
 protected:

  virtual void AuditDestroyingToken (details::Token* token)
  {}

  void PushBackBinding (details::Binding* binding);
    
  void PushFrontBinding (details::Binding* binding);
    
  void InsertBinding (int index, details::Binding* binding);
    
  static inline void link (details::Token* token, details::Binding* binding)
  {
#ifdef DEBUG
    assert((token->binding == nullptr) && (binding->token == nullptr));
#endif
      
    token->binding = binding;
    binding->token = token;
  }
    
  static inline void push_front (Trackable* trackable,
                                 details::Binding* conn)
  {
    trackable->PushBackBinding(conn);
  }
    
  static inline void push_back (Trackable* trackable,
                                details::Binding* conn)
  {
    trackable->PushBackBinding(conn);
  }
    
  static inline void insert (Trackable* trackable,
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
  

// Event declaration:

template<typename ... ParamTypes>
class Event: public Trackable
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
  void Connect (T* obj, void (T::*method) (const Meta*, ParamTypes...), int index = -1);

  void Connect (Event<ParamTypes...>& other, int index = -1);

  /**
   * @brief Disconnect all delegates to a method
   */
  template<typename T>
  void DisconnectAll (T* obj, void (T::*method) (const Meta*, ParamTypes...));

  /**
   * @brief Disconnect all events
   */
  void DisconnectAll (Event<ParamTypes...>& other);

  /**
   * @brief Disconnect the last delegate to a method
   */
  template<typename T>
  void DisconnectOnce (T* obj, void (T::*method) (const Meta*, ParamTypes...), int start_pos = -1);

  /**
   * @brief Disconnect the last event
   */
  void DisconnectOnce (Event<ParamTypes...>& other, int start_pos = -1);

  void RemoveAllOutConnections ();

  template<typename T>
  bool IsConnected (T* obj, void (T::*method) (const Meta*, ParamTypes...)) const;

  bool IsConnected (Event<ParamTypes...>& other) const;

  template<typename T>
  std::size_t CountConnections (T* obj, void (T::*method) (const Meta*, ParamTypes...)) const;

  std::size_t CountConnections (Event<ParamTypes...>& other) const;

  std::size_t CountOutConnections () const;

  void Fire (ParamTypes ... Args);
  
  inline void operator () (ParamTypes ... Args);

 protected:

  virtual void AuditDestroyingToken (details::Token* token) final;
  
  void PushBackToken (details::Token* token);

  void PushFrontToken (details::Token* token);

  void InsertToken (int index, details::Token* token);

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

  details::Token* first_token_;
  details::Token* last_token_;
};

// Event implementation:

template<typename ... ParamTypes>
inline Event<ParamTypes...>::Event ()
    : Trackable(),
      first_token_(nullptr),
      last_token_(nullptr)
{
}

template<typename ... ParamTypes>
Event<ParamTypes...>::~Event ()
{
  RemoveAllOutConnections();
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::Connect (T* obj, void (T::*method) (const Meta*, ParamTypes...), int index)
{
  details::Binding* downstream = new details::Binding;
  Delegate<void, const Meta*, ParamTypes...> d =
      Delegate<void, const Meta*, ParamTypes...>::template from_method<T>(obj, method);
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
void Event<ParamTypes...>::DisconnectAll (T* obj, void (T::*method) (const Meta*, ParamTypes...))
{
  details::DelegateToken<const Meta*, ParamTypes...>* conn = nullptr;

  details::Token* p1 = nullptr;
  details::Token* p2 = nullptr;

  p1 = last_token_;
  while(p1) {
    p2 = p1->previous;
    conn = dynamic_cast<details::DelegateToken<const Meta*, ParamTypes...>*>(p1);
    if (conn && (conn->delegate().template equal<T>(obj, method))) {
      delete conn;
    }
    p1 = p2;
  }

}
  
template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAll (Event<ParamTypes...>& other)
{
  details::EventToken<ParamTypes...>* conn = nullptr;

  details::Token* p1 = nullptr;
  details::Token* p2 = nullptr;

  p1 = last_token_;
  while(p1) {
    p2 = p1->previous;
    conn = dynamic_cast<details::EventToken<ParamTypes...>*>(p1);
    if (conn && (conn->event() == (&other))) {
      delete conn;
    }
    p1 = p2;
  }

}
  
template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::DisconnectOnce (T* obj, void (T::*method) (const Meta*, ParamTypes...), int start_pos)
{
  details::DelegateToken<const Meta*, ParamTypes...>* conn = nullptr;
  details::Token* start = nullptr;

  if (start_pos >= 0) {
    start = first_token_;
    
    while (start && (start_pos > 0)) {
      start = start->next;
      start_pos--;
    }
    
    if (start) {
      for (details::Token* p = start; p; p = p->next) {
        conn = dynamic_cast<details::DelegateToken<const Meta*, ParamTypes...>*>(p);
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
        conn = dynamic_cast<details::DelegateToken<const Meta*, ParamTypes...>*>(p);
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
  details::EventToken<ParamTypes...>* conn = nullptr;
  details::Token* start = nullptr;
  
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
bool Event<ParamTypes...>::IsConnected (T* obj, void (T::*method) (const Meta*, ParamTypes...)) const
{
  details::DelegateToken<const Meta*, ParamTypes...>* conn = nullptr;

  for(details::Token* p = first_token_; p; p = p->next) {
    conn = dynamic_cast<details::DelegateToken<const Meta*, ParamTypes...>*>(p);
    if (conn && (conn->delegate().template equal<T>(obj, method))) {
      return true;
    }
  }
  return false;
}

template<typename ... ParamTypes>
bool Event<ParamTypes...>::IsConnected (Event<ParamTypes...>& other) const
{
  details::EventToken<ParamTypes...>* conn = nullptr;

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
std::size_t Event<ParamTypes...>::CountConnections (T* obj, void (T::*method) (const Meta*, ParamTypes...)) const
{
  std::size_t count = 0;
  details::DelegateToken<const Meta*, ParamTypes...>* conn = nullptr;

  for(details::Token* p = first_token_; p; p = p->next) {
    conn = dynamic_cast<details::DelegateToken<const Meta*, ParamTypes...>*>(p);
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
  details::EventToken<ParamTypes...>* conn = nullptr;

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
  Meta m;
  m.iterator_ = first_token_;
  
  while (m.iterator_) {
    static_cast<details::InvokableToken<ParamTypes...>*>(m.iterator_)->Invoke(&m, Args...);

    m.iterator_ = m.iterator_->next;
  }
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
}
  
template<typename ... ParamTypes>
void Event<ParamTypes...>::PushBackToken(details::Token* token)
{
#ifdef DEBUG
  assert(token->trackable_object == nullptr);
#endif

  if (last_token_) {
    last_token_->next = token;
    token->previous = last_token_;
  } else {
#ifdef DEBUG
    assert(first_token_ == nullptr);
#endif
    token->previous = nullptr;
    first_token_ = token;
  }
  last_token_ = token;
  token->next = nullptr;
  token->trackable_object = this;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::PushFrontToken(details::Token* token)
{
#ifdef DEBUG
  assert(token->trackable_object == nullptr);
#endif

  if (first_token_) {
    first_token_->previous = token;
    token->next = first_token_;
  } else {
#ifdef DEBUG
    assert(last_token_ == nullptr);
#endif
    token->next = nullptr;
    last_token_ = token;
  }
  first_token_ = token;

  token->previous = nullptr;
  token->trackable_object = this;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::InsertToken(int index, details::Token* token)
{
#ifdef DEBUG
  assert(token->trackable_object == nullptr);
#endif
  
  if (first_token_ == nullptr) {
#ifdef DEBUG
    assert(last_token_ == nullptr);
#endif
    token->next = nullptr;
    last_token_ = token;
    first_token_ = token;
    token->previous = nullptr;
  } else {
    if (index >= 0) {
      
      details::Token* p = first_token_;
#ifdef DEBUG
      assert(p != nullptr);
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
        token->next = nullptr;
        
      }
      
    } else {
      
      details::Token* p = last_token_;
#ifdef DEBUG
      assert(p != nullptr);
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
        token->previous = nullptr;
        
      }
      
    }
  }
  token->trackable_object = this;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::RemoveAllOutConnections()
{
  details::Token* tmp = nullptr;
  details::Token* p = first_token_;

  while (p) {
    tmp = p->next;
    delete p;
    p = tmp;
  }
}

} // namespace CppEvent

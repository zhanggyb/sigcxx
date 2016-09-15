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

namespace cppevent {

// forward declaration
class Trackable;
template<typename ... ParamTypes>
class Event;
class Sender;

namespace details {

struct Token;
template<typename ... ParamTypes>
class EventToken;

/**
 * @brief The event binding
 */
struct Binding {
  inline Binding()
      : trackable_object(nullptr),
        previous(nullptr),
        next(nullptr),
        token(nullptr) {
  }

  ~Binding();

  Trackable *trackable_object;
  Binding *previous;
  Binding *next;
  Token *token;
};

/**
 * @brief The event source
 */
struct Token {
  inline Token()
      : trackable_object(nullptr),
        previous(nullptr),
        next(nullptr),
        binding(nullptr) {
  }

  virtual ~Token();

  Trackable *trackable_object;
  Token *previous;
  Token *next;
  Binding *binding;
};

template<typename ... ParamTypes>
class InvokableToken : public Token {
 public:

  inline InvokableToken();

  virtual ~InvokableToken();

  virtual void Invoke(const Sender *sender, ParamTypes ... Args);
};

template<typename ... ParamTypes>
inline InvokableToken<ParamTypes...>::InvokableToken()
    : Token() {
}

template<typename ... ParamTypes>
InvokableToken<ParamTypes...>::~InvokableToken() {
}

template<typename ... ParamTypes>
void InvokableToken<ParamTypes...>::Invoke(const Sender *sender, ParamTypes ... Args) {
  // Override this in sub class
}

template<typename ... ParamTypes>
class DelegateToken : public InvokableToken<ParamTypes...> {
 public:

  DelegateToken() = delete;

  inline DelegateToken(const Delegate<void, const Sender *, ParamTypes...> &d);

  virtual ~DelegateToken();

  virtual void Invoke(const Sender *sender, ParamTypes... Args) override;

  const Delegate<void, const Sender *, ParamTypes...> &delegate() const {
    return delegate_;
  }

 private:

  Delegate<void, const Sender *, ParamTypes...> delegate_;

};

template<typename ... ParamTypes>
inline DelegateToken<ParamTypes...>::DelegateToken(const Delegate<void, const Sender *, ParamTypes...> &d)
    : InvokableToken<ParamTypes...>(), delegate_(d) {
}

template<typename ... ParamTypes>
DelegateToken<ParamTypes...>::~DelegateToken() {
}

template<typename ... ParamTypes>
void DelegateToken<ParamTypes...>::Invoke(const Sender *sender, ParamTypes... Args) {
  delegate_(sender, Args...);
}

template<typename ... ParamTypes>
class EventToken : public InvokableToken<ParamTypes...> {
 public:

  EventToken() = delete;

  inline EventToken(Event<ParamTypes...> &event);

  virtual ~EventToken();

  virtual void Invoke(const Sender *sender, ParamTypes... Args) override;

  inline const Event<ParamTypes...> *event() const;

 private:

  Event<ParamTypes...> *event_;
};

template<typename ... ParamTypes>
inline EventToken<ParamTypes...>::EventToken(Event<ParamTypes...> &event)
    : InvokableToken<ParamTypes...>(), event_(&event) {
}

template<typename ... ParamTypes>
EventToken<ParamTypes...>::~EventToken() {
}

template<typename ... ParamTypes>
void EventToken<ParamTypes...>::Invoke(const Sender *sender, ParamTypes... Args) {
  event_->Fire(Args...);
}

template<typename ... ParamTypes>
inline const Event<ParamTypes...> *EventToken<ParamTypes...>::event() const {
  return event_;
}

}// namespace details

class Sender {

  template<typename ... ParamTypes> friend
  class Event;
  template<typename ... ParamTypes> friend
  class details::EventToken;
  friend class Trackable;

 public:

  ~Sender() {}

  template<typename ... ParamTypes>
  inline Event<ParamTypes...> *event() const {
    return dynamic_cast<Event<ParamTypes...> *>(token_->trackable_object);
  }

 private:

  inline Sender(details::Token *token = nullptr, bool skip = false)
      : token_(token), skip_(skip) {}

  details::Token *token_;
  bool skip_;

};

/**
 * @brief The basic class for observer
 */
class Trackable {
  friend struct details::Binding;
  friend struct details::Token;
  template<typename ... ParamTypes> friend
  class Event;

 public:

  inline Trackable()
      : first_binding_(nullptr), last_binding_(nullptr) {
  }

  virtual ~Trackable();

  inline Trackable(const Trackable &orig) {}

  inline Trackable &operator=(const Trackable &orig) {
    return *this;
  }

  void UnbindOnce(const Sender *sender);

  void UnbindAll(const Sender *sender);

  template<typename T, typename ... ParamTypes>
  void UnbindAll(const Sender *sender, void (T::*method)(const Sender *, ParamTypes...));

  void UnbindAll();

  /**
   * @brief Disconnect all delegates to a method
   */
  template<typename T, typename ... ParamTypes>
  void UnbindAll(void (T::*method)(const Sender *, ParamTypes...));

  template<typename T, typename ... ParamTypes>
  std::size_t CountBindings(T *obj, void (T::*method)(const Sender *, ParamTypes...)) const;

  std::size_t CountBindings() const;

 protected:

  virtual void AuditDestroyingToken(details::Token *token) {}

  void PushBackBinding(details::Binding *binding);

  void PushFrontBinding(details::Binding *binding);

  void InsertBinding(int index, details::Binding *binding);

  static inline void link(details::Token *token, details::Binding *binding) {
#ifdef DEBUG
    assert((token->binding == nullptr) && (binding->token == nullptr));
#endif

    token->binding = binding;
    binding->token = token;
  }

  static inline void push_front(Trackable *trackable,
                                details::Binding *conn) {
    trackable->PushBackBinding(conn);
  }

  static inline void push_back(Trackable *trackable,
                               details::Binding *conn) {
    trackable->PushBackBinding(conn);
  }

  static inline void insert(Trackable *trackable,
                            details::Binding *conn,
                            int index = 0) {
    trackable->InsertBinding(index, conn);
  }

  inline details::Binding *first_binding() const {
    return first_binding_;
  }

  inline details::Binding *last_binding() const {
    return last_binding_;
  }

 private:

  details::Binding *first_binding_;
  details::Binding *last_binding_;
};

template<typename T, typename ... ParamTypes>
void Trackable::UnbindAll(const Sender *sender, void (T::*method)(const Sender *, ParamTypes...)) {
  // (sender && sender->token_->binding->trackable_object == this) is always true

  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;

  details::Token *p = nullptr;
  details::Token *tmp = nullptr;

  p = sender->token_;
  while (p) {
    tmp = p->previous;
    if (p->binding->trackable_object == this) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(p);
      if (delegate_token &&
          (delegate_token->delegate().template equal<T>((T *) this, method))) {
        if (p == sender->token_) {
          const_cast<Sender *>(sender)->token_ = sender->token_->next;
          const_cast<Sender *>(sender)->skip_ = true;
        }
        delete p;
      }
    }
    p = tmp;
  }

  p = sender->token_;
  while (p) {
    tmp = p->next;
    if (p->binding->trackable_object == this) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(p);
      if (delegate_token &&
          (delegate_token->delegate().template equal<T>((T *) this, method))) {
        if (p == sender->token_) {
          const_cast<Sender *>(sender)->token_ = sender->token_->next;
          const_cast<Sender *>(sender)->skip_ = true;
        }
        delete p;
      }
    }
    p = tmp;
  }
}

template<typename T, typename ... ParamTypes>
void Trackable::UnbindAll(void (T::*method)(const Sender *, ParamTypes...)) {

  details::Binding *p = nullptr;
  details::Binding *tmp = nullptr;
  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;

  p = last_binding_;
  while (p) {
    tmp = p->previous;
    delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(p->token);
    if (delegate_token && (delegate_token->delegate().template equal<T>((T *) this, method))) {
      delete delegate_token;
    }
    p = tmp;
  }

}

template<typename T, typename ... ParamTypes>
size_t Trackable::CountBindings(T *obj, void (T::*method)(const Sender *, ParamTypes...)) const {
  std::size_t count = 0;
  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;

  for (details::Binding *p = first_binding_; p; p = p->next) {
    delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(p->token);
    if (delegate_token && (delegate_token->delegate().template equal<T>(obj, method))) {
      count++;
    }
  }
  return count;
}

// Event declaration:

template<typename ... ParamTypes>
class Event : public Trackable {
  friend class Trackable;

 public:

  inline Event();

  virtual ~Event();

  Event(const Event &orig) = delete;

  Event &operator=(const Event &orig) = delete;

  /**
   * @brief Connect this event to a method in a observer
   */
  template<typename T>
  void Connect(T *obj, void (T::*method)(const Sender *, ParamTypes...), int index = -1);

  void Connect(Event<ParamTypes...> &other, int index = -1);

  /**
   * @brief Disconnect all delegates to a method
   */
  template<typename T>
  void DisconnectAll(T *obj, void (T::*method)(const Sender *, ParamTypes...));

  /**
   * @brief Disconnect all events
   */
  void DisconnectAll(Event<ParamTypes...> &other);

  /**
   * @brief Disconnect the last delegate to a method
   */
  template<typename T>
  void DisconnectOnce(T *obj, void (T::*method)(const Sender *, ParamTypes...), int start_pos = -1);

  /**
   * @brief Disconnect the last event
   */
  void DisconnectOnce(Event<ParamTypes...> &other, int start_pos = -1);

  void DisconnectAll();

  template<typename T>
  bool IsConnected(T *obj, void (T::*method)(const Sender *, ParamTypes...)) const;

  bool IsConnected(const Event<ParamTypes...> &other) const;

  bool IsConnected(const Trackable *obj) const;

  template<typename T>
  std::size_t CountConnections(T *obj, void (T::*method)(const Sender *, ParamTypes...)) const;

  std::size_t CountConnections(const Event<ParamTypes...> &other) const;

  std::size_t CountConnections() const;

  void Fire(ParamTypes ... Args);

  inline void operator()(ParamTypes ... Args);

 protected:

  virtual void AuditDestroyingToken(details::Token *token) final;

  void PushBackToken(details::Token *token);

  void PushFrontToken(details::Token *token);

  void InsertToken(int index, details::Token *token);

  inline details::Token *first_token() const {
    return first_token_;
  }

  inline details::Token *last_token() const {
    return last_token_;
  }

 private:

  static inline void set_bit(int &v, int m) { v |= m; }
  static inline void clear_bit(int &v, int m) { v &= ~m; }

  details::Token *first_token_;
  details::Token *last_token_;
};

// Event implementation:

template<typename ... ParamTypes>
inline Event<ParamTypes...>::Event()
    : Trackable(),
      first_token_(nullptr),
      last_token_(nullptr) {
}

template<typename ... ParamTypes>
Event<ParamTypes...>::~Event() {
  DisconnectAll();
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::Connect(T *obj, void (T::*method)(const Sender *, ParamTypes...), int index) {
  details::Binding *downstream = new details::Binding;
  Delegate<void, const Sender *, ParamTypes...> d =
      Delegate<void, const Sender *, ParamTypes...>::template from_method<T>(obj, method);
  details::DelegateToken<ParamTypes...> *upstream = new details::DelegateToken<
      ParamTypes...>(d);

  link(upstream, downstream);
  InsertToken(index, upstream);
  push_back(obj, downstream);  // always push back binding, don't care about the position in observer
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Connect(Event<ParamTypes...> &other, int index) {
  details::EventToken<ParamTypes...> *upstream = new details::EventToken<ParamTypes...>(
      other);
  details::Binding *downstream = new details::Binding;

  link(upstream, downstream);
  InsertToken(index, upstream);
  push_back(&other, downstream);  // always push back binding, don't care about the position in observer
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::DisconnectAll(T *obj, void (T::*method)(const Sender *, ParamTypes...)) {
  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;

  details::Token *p = nullptr;
  details::Token *tmp = nullptr;

  p = last_token_;
  while (p) {
    tmp = p->previous;
    if (p->binding->trackable_object == obj) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> * > (p);
      if (delegate_token && (delegate_token->delegate().template equal<T>(obj, method))) {
        delete delegate_token;
      }
    }
    p = tmp;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAll(Event<ParamTypes...> &other) {
  details::EventToken<ParamTypes...> *event_token = nullptr;

  details::Token *p = nullptr;
  details::Token *tmp = nullptr;

  p = last_token_;
  while (p) {
    tmp = p->previous;
    if (p->binding->trackable_object == (&other)) {
      event_token = dynamic_cast<details::EventToken<ParamTypes...> * > (p);
      if (event_token && (event_token->event() == (&other))) {
        delete event_token;
      }
    }
    p = tmp;
  }
}

template<typename ... ParamTypes>
template<typename T>
void Event<ParamTypes...>::DisconnectOnce(T *obj, void (T::*method)(const Sender *, ParamTypes...), int start_pos) {
  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;
  details::Token *start = nullptr;

  if (start_pos >= 0) {
    start = first_token_;

    while (start && (start_pos > 0)) {
      start = start->next;
      start_pos--;
    }

    if (start) {
      for (details::Token *p = start; p; p = p->next) {
        if (p->binding->trackable_object == obj) {
          delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> * > (p);
          if (delegate_token && (delegate_token->delegate().template equal<T>(obj, method))) {
            delete delegate_token;
            break;
          }
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
      for (details::Token *p = last_token_; p; p = p->previous) {
        if (p->binding->trackable_object == obj) {
          delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> * > (p);
          if (delegate_token && (delegate_token->delegate().template equal<T>(obj, method))) {
            delete delegate_token;
            break;
          }
        }
      }
    }

  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectOnce(Event<ParamTypes...> &other, int start_pos) {
  details::EventToken<ParamTypes...> *event_token = nullptr;
  details::Token *start = nullptr;

  if (start_pos >= 0) {
    start = first_token_;

    while (start && (start_pos > 0)) {
      start = start->next;
      start_pos--;
    }

    if (start) {
      for (details::Token *p = start; p; p = p->next) {
        if (p->binding->trackable_object == (&other)) {
          event_token = dynamic_cast<details::EventToken<ParamTypes...> * > (p);
          if (event_token && (event_token->event() == (&other))) {
            delete event_token;
            break;
          }
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
      for (details::Token *p = last_token_; p; p = p->previous) {
        if (p->binding->trackable_object == (&other)) {
          event_token = dynamic_cast<details::EventToken<ParamTypes...> * > (p);
          if (event_token && (event_token->event() == (&other))) {
            delete event_token;
            break;
          }
        }
      }
    }

  }
}

template<typename ... ParamTypes>
template<typename T>
bool Event<ParamTypes...>::IsConnected(T *obj, void (T::*method)(const Sender *, ParamTypes...)) const {
  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;

  for (details::Token *p = first_token_; p; p = p->next) {
    if (p->binding->trackable_object == obj) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(p);
      if (delegate_token && (delegate_token->delegate().template equal<T>(obj, method))) {
        return true;
      }
    }
  }
  return false;
}

template<typename ... ParamTypes>
bool Event<ParamTypes...>::IsConnected(const Event<ParamTypes...> &other) const {
  details::EventToken<ParamTypes...> *event_token = nullptr;

  for (details::Token *p = first_token_; p; p = p->next) {
    if (p->binding->trackable_object == (&other)) {
      event_token = dynamic_cast<details::EventToken<ParamTypes...> * > (p);
      if (event_token && (event_token->event() == (&other))) {
        return true;
      }
    }
  }
  return false;
}

template<typename ... ParamTypes>
bool Event<ParamTypes...>::IsConnected(const Trackable *obj) const {
  details::Token *token = first_token();
  details::Binding *binding = obj->first_binding();

  while (token && binding) {

    if (token->binding->trackable_object == obj) return true;
    if (binding->token->trackable_object == this) return true;

    token = token->next;
    binding = binding->next;
  }

  return false;
}

template<typename ... ParamTypes>
template<typename T>
std::size_t Event<ParamTypes...>::CountConnections(T *obj, void (T::*method)(const Sender *, ParamTypes...)) const {
  std::size_t count = 0;
  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;

  for (details::Token *p = first_token_; p; p = p->next) {
    if (p->binding->trackable_object == obj) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(p);
      if (delegate_token && (delegate_token->delegate().template equal<T>(obj, method))) {
        count++;
      }
    }
  }
  return count;
}

template<typename ... ParamTypes>
std::size_t Event<ParamTypes...>::CountConnections(const Event<ParamTypes...> &other) const {
  std::size_t count = 0;
  details::EventToken<ParamTypes...> *event_token = nullptr;

  for (details::Token *p = first_token_; p; p = p->next) {
    if (p->binding->trackable_object == (&other)) {
      event_token = dynamic_cast<details::EventToken<ParamTypes...> * > (p);
      if (event_token && (event_token->event() == (&other))) {
        count++;
      }
    }
  }
  return count;
}

template<typename ... ParamTypes>
std::size_t Event<ParamTypes...>::CountConnections() const {
  std::size_t count = 0;
  for (details::Token *p = first_token_; p; p = p->next) {
    count++;
  }
  return count;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Fire(ParamTypes ... Args) {
  Sender sender(first_token());

  while (sender.token_) {
    static_cast<details::InvokableToken<ParamTypes...> * > (sender.token_)->Invoke(&sender, Args...);

    if (sender.skip_) {
      sender.skip_ = false;
    } else {
      sender.token_ = sender.token_->next;
    }
  }
}

template<typename ... ParamTypes>
inline void Event<ParamTypes...>::operator()(ParamTypes ... Args) {
  Fire(Args...);
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::AuditDestroyingToken(details::Token *token) {
  if (token == first_token_) first_token_ = token->next;
  if (token == last_token_) last_token_ = token->previous;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::PushBackToken(details::Token *token) {
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
void Event<ParamTypes...>::PushFrontToken(details::Token *token) {
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
void Event<ParamTypes...>::InsertToken(int index, details::Token *token) {
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

      details::Token *p = first_token_;
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

      details::Token *p = last_token_;
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
void Event<ParamTypes...>::DisconnectAll() {
  details::Token *tmp = nullptr;
  details::Token *p = first_token_;

  while (p) {
    tmp = p->next;
    delete p;
    p = tmp;
  }
}

/**
 * @brief A reference to expose event in an object
 */
template<typename ... ParamTypes>
class EventRef {
 public:

  EventRef() = delete;

  inline EventRef(Event<ParamTypes...> &event)
      : event_(&event) {
  }

  inline EventRef(Event<ParamTypes...> *event)
      : event_(event) {
  }

  inline EventRef(const EventRef &orig)
      : event_(orig.event_) {
  }

  ~EventRef() {}

  inline EventRef &operator=(const EventRef &orig) {
    event_ = orig.event_;
    return *this;
  }

  template<typename T>
  inline void connect(T *obj, void (T::*method)(const Sender *, ParamTypes...), int index = -1) {
    event_->Connect(obj, method, index);
  }

  inline void connect(const EventRef<ParamTypes...> &other, int index = -1) {
    event_->Connect(*other.event_, index);
  }

  template<typename T>
  inline void disconnect_all(T *obj, void (T::*method)(const Sender *, ParamTypes...)) {
    event_->DisconnectAll(obj, method);
  }

  inline void disconnect_all(EventRef<ParamTypes...> &other) {
    event_->DisconnectAll(*other.event_);
  }

  template<typename T>
  inline void disconnect_once(T *obj, void (T::*method)(const Sender *, ParamTypes...), int start_pos = -1) {
    event_->DisconnectOnce(obj, method, start_pos);
  }

  inline void disconnect_once(EventRef<ParamTypes...> &other, int start_pos = -1) {
    event_->DisconnectOnce(*other.event_, start_pos);
  }

  inline void disconnect_all() {
    event_->DisconnectAll();
  }

  template<typename T>
  inline bool is_connected(T *obj, void (T::*method)(const Sender *, ParamTypes...)) const {
    return event_->IsConnected(obj, method);
  }

  inline bool is_connected(const EventRef<ParamTypes...> &other) const {
    return event_->IsConnected(*other.event_);
  }

  inline bool is_connected(const Trackable *obj) const {
    return event_->IsConnected(obj);
  }

  template<typename T>
  inline std::size_t count_connections(T *obj, void (T::*method)(const Sender *, ParamTypes...)) const {
    return event_->CountConnections(obj, method);
  }

  inline std::size_t count_connections(const EventRef<ParamTypes...> &other) const {
    return event_->CountConnections(*other.event_);
  }

  inline std::size_t count_connections() const {
    return event_->CountConnections();
  }

  template<typename T>
  inline std::size_t count_bindings(T *obj, void (T::*method)(const Sender *, ParamTypes...)) const {
    return event_->CountBindings(obj, method);
  }

  inline std::size_t count_bindings() const {
    return event_->CountBindings();
  }

 private:

  Event<ParamTypes...> *event_;

};

} // namespace cppevent

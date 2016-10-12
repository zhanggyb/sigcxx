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

#ifndef SIGCXX_SIGCXX_HPP_
#define SIGCXX_SIGCXX_HPP_

#ifdef DEBUG
#include <cassert>
#endif

#include <cstddef>

#include "delegate.hpp"

namespace sigcxx {

// forward declaration
class Trackable;
template<typename ... ParamTypes>
class Signal;
class Slot;

typedef const Slot *SLOT;

namespace details {

struct Token;
template<typename ... ParamTypes>
class SignalToken;

/**
 * @brief A simple structure to build a list point to signal sources
 */
struct Binding {

  /**
   * @brief Constructor
   */
  inline Binding()
      : trackable_object(nullptr),
        previous(nullptr),
        next(nullptr),
        token(nullptr) {
  }

  /**
   * @brief Destructor
   */
  ~Binding();

  Trackable *trackable_object;
  Binding *previous;
  Binding *next;
  Token *token;
};

/**
 * @brief A simple structure to build a list point to objects which consume the signals
 */
struct Token {

  /**
   * @brief Constructor
   */
  inline Token()
      : trackable_object(nullptr),
        previous(nullptr),
        next(nullptr),
        binding(nullptr) {
  }

  /**
   * @brief Destructor
   */
  virtual ~Token();

  Trackable *trackable_object;
  Token *previous;
  Token *next;
  Binding *binding;
};

template<typename ... ParamTypes>
class CallableToken : public Token {
 public:

  inline CallableToken();

  virtual ~CallableToken();

  virtual void Invoke(SLOT slot, ParamTypes ... Args);
};

template<typename ... ParamTypes>
inline CallableToken<ParamTypes...>::CallableToken()
    : Token() {
}

template<typename ... ParamTypes>
CallableToken<ParamTypes...>::~CallableToken() {
}

template<typename ... ParamTypes>
void CallableToken<ParamTypes...>::Invoke(SLOT slot, ParamTypes ... Args) {
  // Override this in sub class
}

template<typename ... ParamTypes>
class DelegateToken : public CallableToken<ParamTypes...> {
 public:

  DelegateToken() = delete;

  inline DelegateToken(const Delegate<void(SLOT, ParamTypes...)> &d);

  virtual ~DelegateToken();

  virtual void Invoke(SLOT slot, ParamTypes... Args) override;

  const Delegate<void(SLOT, ParamTypes...)> &delegate() const {
    return delegate_;
  }

 private:

  Delegate<void(SLOT, ParamTypes...)> delegate_;

};

template<typename ... ParamTypes>
inline DelegateToken<ParamTypes...>::DelegateToken(const Delegate<void(SLOT, ParamTypes...)> &d)
    : CallableToken<ParamTypes...>(), delegate_(d) {
}

template<typename ... ParamTypes>
DelegateToken<ParamTypes...>::~DelegateToken() {
}

template<typename ... ParamTypes>
void DelegateToken<ParamTypes...>::Invoke(SLOT slot, ParamTypes... Args) {
  delegate_(slot, Args...);
}

template<typename ... ParamTypes>
class SignalToken : public CallableToken<ParamTypes...> {
 public:

  SignalToken() = delete;

  inline SignalToken(Signal<ParamTypes...> &other);

  virtual ~SignalToken();

  virtual void Invoke(SLOT slot, ParamTypes... Args) override;

  inline const Signal<ParamTypes...> *signal() const;

 private:

  Signal<ParamTypes...> *signal_;
};

template<typename ... ParamTypes>
inline SignalToken<ParamTypes...>::SignalToken(Signal<ParamTypes...> &other)
    : CallableToken<ParamTypes...>(), signal_(&other) {
}

template<typename ... ParamTypes>
SignalToken<ParamTypes...>::~SignalToken() {
}

template<typename ... ParamTypes>
void SignalToken<ParamTypes...>::Invoke(SLOT slot, ParamTypes... Args) {
  signal_->Emit(Args...);
}

template<typename ... ParamTypes>
inline const Signal<ParamTypes...> *SignalToken<ParamTypes...>::signal() const {
  return signal_;
}

}// namespace details

class Slot {

  template<typename ... ParamTypes> friend
  class Signal;
  template<typename ... ParamTypes> friend
  class details::SignalToken;
  friend class Trackable;

 public:

  ~Slot() {}

  template<typename ... ParamTypes>
  inline Signal<ParamTypes...> *signal() const {
    return dynamic_cast<Signal<ParamTypes...> *>(token_->trackable_object);
  }

 private:

  inline Slot(details::Token *token = nullptr, bool skip = false)
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
  class Signal;

 public:

  inline Trackable()
      : first_binding_(nullptr), last_binding_(nullptr) {
  }

  virtual ~Trackable();

  inline Trackable(const Trackable &orig) {}

  inline Trackable &operator=(const Trackable &orig) {
    return *this;
  }

  void UnbindOnce(SLOT slot);

  void UnbindAll(SLOT slot);

  template<typename T, typename ... ParamTypes>
  void UnbindAll(SLOT slot, void (T::*method)(SLOT, ParamTypes...));

  void UnbindAll();

  /**
   * @brief Disconnect all delegates to a method
   */
  template<typename T, typename ... ParamTypes>
  void UnbindAll(void (T::*method)(SLOT, ParamTypes...));

  template<typename T, typename ... ParamTypes>
  std::size_t CountBindings(T *obj, void (T::*method)(SLOT, ParamTypes...)) const;

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
void Trackable::UnbindAll(SLOT slot, void (T::*method)(SLOT, ParamTypes...)) {
  // (slot && slot->token_->binding->trackable_object == this) is always true

  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;

  details::Token *p = nullptr;
  details::Token *tmp = nullptr;

  p = slot->token_;
  while (p) {
    tmp = p->previous;
    if (p->binding->trackable_object == this) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(p);
      if (delegate_token &&
          (delegate_token->delegate().template equal<T>((T *) this, method))) {
        if (p == slot->token_) {
          const_cast<Slot *>(slot)->token_ = slot->token_->next;
          const_cast<Slot *>(slot)->skip_ = true;
        }
        delete p;
      }
    }
    p = tmp;
  }

  p = slot->token_;
  while (p) {
    tmp = p->next;
    if (p->binding->trackable_object == this) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(p);
      if (delegate_token &&
          (delegate_token->delegate().template equal<T>((T *) this, method))) {
        if (p == slot->token_) {
          const_cast<Slot *>(slot)->token_ = slot->token_->next;
          const_cast<Slot *>(slot)->skip_ = true;
        }
        delete p;
      }
    }
    p = tmp;
  }
}

template<typename T, typename ... ParamTypes>
void Trackable::UnbindAll(void (T::*method)(SLOT, ParamTypes...)) {

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
size_t Trackable::CountBindings(T *obj, void (T::*method)(SLOT, ParamTypes...)) const {
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

// Signal declaration:

template<typename ... ParamTypes>
class Signal : public Trackable {
  friend class Trackable;

 public:

  inline Signal();

  virtual ~Signal();

  Signal(const Signal &orig) = delete;

  Signal &operator=(const Signal &orig) = delete;

  /**
   * @brief Connect this signal to a slot method in a observer
   */
  template<typename T>
  void Connect(T *obj, void (T::*method)(SLOT, ParamTypes...), int index = -1);

  void Connect(Signal<ParamTypes...> &other, int index = -1);

  /**
   * @brief Disconnect all delegates to a method
   */
  template<typename T>
  void DisconnectAll(T *obj, void (T::*method)(SLOT, ParamTypes...));

  /**
   * @brief Disconnect all signals
   */
  void DisconnectAll(Signal<ParamTypes...> &other);

  /**
   * @brief Disconnect the last delegate to a method
   */
  template<typename T>
  void DisconnectOnce(T *obj, void (T::*method)(SLOT, ParamTypes...), int start_pos = -1);

  /**
   * @brief Disconnect the last signal
   */
  void DisconnectOnce(Signal<ParamTypes...> &other, int start_pos = -1);

  void DisconnectAll();

  template<typename T>
  bool IsConnected(T *obj, void (T::*method)(SLOT, ParamTypes...)) const;

  bool IsConnected(const Signal<ParamTypes...> &other) const;

  bool IsConnected(const Trackable *obj) const;

  template<typename T>
  std::size_t CountConnections(T *obj, void (T::*method)(SLOT, ParamTypes...)) const;

  std::size_t CountConnections(const Signal<ParamTypes...> &other) const;

  std::size_t CountConnections() const;

  void Emit(ParamTypes ... Args);

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

  details::Token *first_token_;
  details::Token *last_token_;
};

// Signal implementation:

template<typename ... ParamTypes>
inline Signal<ParamTypes...>::Signal()
    : Trackable(),
      first_token_(nullptr),
      last_token_(nullptr) {
}

template<typename ... ParamTypes>
Signal<ParamTypes...>::~Signal() {
  DisconnectAll();
}

template<typename ... ParamTypes>
template<typename T>
void Signal<ParamTypes...>::Connect(T *obj, void (T::*method)(SLOT, ParamTypes...), int index) {
  details::Binding *downstream = new details::Binding;
  Delegate<void(SLOT, ParamTypes...)> d =
      Delegate<void(SLOT, ParamTypes...)>::template from_method<T>(obj, method);
  details::DelegateToken<ParamTypes...> *upstream = new details::DelegateToken<
      ParamTypes...>(d);

  link(upstream, downstream);
  InsertToken(index, upstream);
  push_back(obj, downstream);  // always push back binding, don't care about the position in observer
}

template<typename ... ParamTypes>
void Signal<ParamTypes...>::Connect(Signal<ParamTypes...> &other, int index) {
  details::SignalToken<ParamTypes...> *upstream = new details::SignalToken<ParamTypes...>(
      other);
  details::Binding *downstream = new details::Binding;

  link(upstream, downstream);
  InsertToken(index, upstream);
  push_back(&other, downstream);  // always push back binding, don't care about the position in observer
}

template<typename ... ParamTypes>
template<typename T>
void Signal<ParamTypes...>::DisconnectAll(T *obj, void (T::*method)(SLOT, ParamTypes...)) {
  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;
  details::Token *p = last_token_;
  details::Token *tmp = nullptr;

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
void Signal<ParamTypes...>::DisconnectAll(Signal<ParamTypes...> &other) {
  details::SignalToken<ParamTypes...> *signal_token = nullptr;
  details::Token *p = last_token_;
  details::Token *tmp = nullptr;

  while (p) {
    tmp = p->previous;
    if (p->binding->trackable_object == (&other)) {
      signal_token = dynamic_cast<details::SignalToken<ParamTypes...> * > (p);
      if (signal_token && (signal_token->signal() == (&other))) {
        delete signal_token;
      }
    }
    p = tmp;
  }
}

template<typename ... ParamTypes>
template<typename T>
void Signal<ParamTypes...>::DisconnectOnce(T *obj, void (T::*method)(SLOT, ParamTypes...), int start_pos) {
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
void Signal<ParamTypes...>::DisconnectOnce(Signal<ParamTypes...> &other, int start_pos) {
  details::SignalToken<ParamTypes...> *signal_token = nullptr;
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
          signal_token = dynamic_cast<details::SignalToken<ParamTypes...> * > (p);
          if (signal_token && (signal_token->signal() == (&other))) {
            delete signal_token;
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
          signal_token = dynamic_cast<details::SignalToken<ParamTypes...> * > (p);
          if (signal_token && (signal_token->signal() == (&other))) {
            delete signal_token;
            break;
          }
        }
      }
    }

  }
}

template<typename ... ParamTypes>
template<typename T>
bool Signal<ParamTypes...>::IsConnected(T *obj, void (T::*method)(SLOT, ParamTypes...)) const {
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
bool Signal<ParamTypes...>::IsConnected(const Signal<ParamTypes...> &other) const {
  details::SignalToken<ParamTypes...> *signal_token = nullptr;

  for (details::Token *p = first_token_; p; p = p->next) {
    if (p->binding->trackable_object == (&other)) {
      signal_token = dynamic_cast<details::SignalToken<ParamTypes...> * > (p);
      if (signal_token && (signal_token->signal() == (&other))) {
        return true;
      }
    }
  }
  return false;
}

template<typename ... ParamTypes>
bool Signal<ParamTypes...>::IsConnected(const Trackable *obj) const {
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
std::size_t Signal<ParamTypes...>::CountConnections(T *obj, void (T::*method)(SLOT, ParamTypes...)) const {
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
std::size_t Signal<ParamTypes...>::CountConnections(const Signal<ParamTypes...> &other) const {
  std::size_t count = 0;
  details::SignalToken<ParamTypes...> *signal_token = nullptr;

  for (details::Token *p = first_token_; p; p = p->next) {
    if (p->binding->trackable_object == (&other)) {
      signal_token = dynamic_cast<details::SignalToken<ParamTypes...> * > (p);
      if (signal_token && (signal_token->signal() == (&other))) {
        count++;
      }
    }
  }
  return count;
}

template<typename ... ParamTypes>
std::size_t Signal<ParamTypes...>::CountConnections() const {
  std::size_t count = 0;
  for (details::Token *p = first_token_; p; p = p->next) {
    count++;
  }
  return count;
}

template<typename ... ParamTypes>
void Signal<ParamTypes...>::Emit(ParamTypes ... Args) {
  Slot slot(first_token());

  while (slot.token_) {
    static_cast<details::CallableToken<ParamTypes...> * > (slot.token_)->Invoke(&slot, Args...);

    if (slot.skip_) {
      slot.skip_ = false;
    } else {
      slot.token_ = slot.token_->next;
    }
  }
}

template<typename ... ParamTypes>
inline void Signal<ParamTypes...>::operator()(ParamTypes ... Args) {
  Emit(Args...);
}

template<typename ... ParamTypes>
void Signal<ParamTypes...>::AuditDestroyingToken(details::Token *token) {
  if (token == first_token_) first_token_ = token->next;
  if (token == last_token_) last_token_ = token->previous;
}

template<typename ... ParamTypes>
void Signal<ParamTypes...>::PushBackToken(details::Token *token) {
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
void Signal<ParamTypes...>::PushFrontToken(details::Token *token) {
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
void Signal<ParamTypes...>::InsertToken(int index, details::Token *token) {
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
void Signal<ParamTypes...>::DisconnectAll() {
  details::Token *tmp = nullptr;
  details::Token *p = first_token_;

  while (p) {
    tmp = p->next;
    delete p;
    p = tmp;
  }
}

/**
 * @brief A reference to expose signal in an object
 */
template<typename ... ParamTypes>
class SignalRef {
 public:

  SignalRef() = delete;

  inline SignalRef(Signal<ParamTypes...> &other)
      : signal_(&other) {
  }

  inline SignalRef(const SignalRef &orig)
      : signal_(orig.signal_) {
  }

  inline ~SignalRef() {}

  inline SignalRef &operator=(const SignalRef &orig) {
    signal_ = orig.signal_;
    return *this;
  }

  template<typename T>
  inline void connect(T *obj, void (T::*method)(SLOT, ParamTypes...), int index = -1) {
    signal_->Connect(obj, method, index);
  }

  inline void connect(Signal<ParamTypes...> &signal, int index = -1) {
    signal_->Connect(signal, index);
  }

  template<typename T>
  inline void disconnect_all(T *obj, void (T::*method)(SLOT, ParamTypes...)) {
    signal_->DisconnectAll(obj, method);
  }

  inline void disconnect_all(Signal<ParamTypes...> &signal) {
    signal_->DisconnectAll(signal);
  }

  template<typename T>
  inline void disconnect_once(T *obj, void (T::*method)(SLOT, ParamTypes...), int start_pos = -1) {
    signal_->DisconnectOnce(obj, method, start_pos);
  }

  inline void disconnect_once(Signal<ParamTypes...> &signal, int start_pos = -1) {
    signal_->DisconnectOnce(signal, start_pos);
  }

  inline void disconnect_all() {
    signal_->DisconnectAll();
  }

  template<typename T>
  inline bool is_connected(T *obj, void (T::*method)(SLOT, ParamTypes...)) const {
    return signal_->IsConnected(obj, method);
  }

  inline bool is_connected(const Signal<ParamTypes...> &signal) const {
    return signal_->IsConnected(signal);
  }

  inline bool is_connected(const Trackable *obj) const {
    return signal_->IsConnected(obj);
  }

  template<typename T>
  inline std::size_t count_connections(T *obj, void (T::*method)(SLOT, ParamTypes...)) const {
    return signal_->CountConnections(obj, method);
  }

  inline std::size_t count_connections(const Signal<ParamTypes...> &signal) const {
    return signal_->CountConnections(signal);
  }

  inline std::size_t count_connections() const {
    return signal_->CountConnections();
  }

  template<typename T>
  inline std::size_t count_bindings(T *obj, void (T::*method)(SLOT, ParamTypes...)) const {
    return signal_->CountBindings(obj, method);
  }

  inline std::size_t count_bindings() const {
    return signal_->CountBindings();
  }

 private:

  Signal<ParamTypes...> *signal_;

};

} // namespace sigcxx

#endif  // SIGCXX_SIGCXX_HPP_
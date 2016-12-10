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

#include <cstddef>
#include "delegate.hpp"

#ifdef DEBUG
#include <cstdio>
#include <cassert>
#endif

#ifndef __SLOT__
#define __SLOT__ SLOT slot = nullptr
#endif

namespace sigcxx {

// forward declaration
class Trackable;
template<typename ... ParamTypes>
class Signal;
class Slot;

typedef Slot *SLOT;

/// @cond IGNORE
namespace details {

struct Token;
template<typename ... ParamTypes>
class SignalToken;

/**
 * @brief A simple structure works as a list node in @ref Trackable object
 */
struct Binding {

  Binding()
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
 * @brief A simple structure works as a list node in @ref Signal object
 * signals
 */
struct Token {

  Token()
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
class CallableToken : public Token {

  CallableToken(const CallableToken &orig) = delete;
  CallableToken &operator=(const CallableToken &other) = delete;

 public:

  CallableToken() : Token() {}

  virtual ~CallableToken() {}

  virtual void Invoke(ParamTypes ... Args);
};

template<typename ... ParamTypes>
void CallableToken<ParamTypes...>::Invoke(ParamTypes ... Args) {
  // Override this in sub class
}

template<typename ... ParamTypes>
class DelegateToken : public CallableToken<ParamTypes...> {

  DelegateToken() = delete;
  DelegateToken(const DelegateToken &orig) = delete;
  DelegateToken &operator=(const DelegateToken &other) = delete;

 public:

  DelegateToken(const Delegate<void(ParamTypes...)> &d)
      : CallableToken<ParamTypes...>(), delegate_(d) {
  }

  virtual ~DelegateToken() {}

  virtual void Invoke(ParamTypes... Args) override;

  inline const Delegate<void(ParamTypes...)> &delegate() const {
    return delegate_;
  }

 private:

  Delegate<void(ParamTypes...)> delegate_;

};

template<typename ... ParamTypes>
void DelegateToken<ParamTypes...>::Invoke(ParamTypes... Args) {
  delegate_(Args...);
}

template<typename ... ParamTypes>
class SignalToken : public CallableToken<ParamTypes...> {

  SignalToken() = delete;
  SignalToken(const SignalToken &orig) = delete;
  SignalToken &operator=(const SignalToken &other) = delete;

 public:

  SignalToken(Signal<ParamTypes...> &signal)
      : CallableToken<ParamTypes...>(), signal_(&signal) {
  }

  virtual ~SignalToken() {}

  virtual void Invoke(ParamTypes... Args) override;

  const Signal<ParamTypes...> *signal() const {
    return signal_;
  }

 private:

  Signal<ParamTypes...> *signal_;
};

template<typename ... ParamTypes>
void SignalToken<ParamTypes...>::Invoke(ParamTypes... Args) {
  signal_->Emit(Args...);
}

}// namespace details
/// @endcond

/**
 * @brief Iterator and signature to a slot method
 *
 * A Slot object is created and destroyed when a signal is being emitting.
 *
 * It has two main purpose:
 *   - Works as an iterator
 *   - The last parameter in a slot method
 *
 * A Signal holds a list of token to support multicast, when it's being
 * emitting, it create a simple Slot object to iterate the list and call each
 * delegate (@ref Delegate) to the slot method.
 *
 * @note If you want to destroy the object in a slot method, you must unbind the
 * signal connection by giving the Slot pointer to @ref Trackable::Unbind(),
 * @ref Trackable::UnbindAll().
 */
class Slot {

  template<typename ... ParamTypes> friend
  class Signal;
  template<typename ... ParamTypes> friend
  class details::SignalToken;
  friend class Trackable;

  Slot() = delete;
  Slot(const Slot &orig) = delete;
  Slot &operator=(const Slot &other) = delete;

 public:

  /**
   * @brief Get the Signal object which is just calling this slot
   */
  template<typename ... ParamTypes>
  Signal<ParamTypes...> *signal() const {
    return dynamic_cast<Signal<ParamTypes...> *>(token_->trackable_object);
  }

  /**
   * @brief The trackable object in which the slot method is being called
   * @return The trackable object receiving signal
   */
  Trackable *object() const {
    return token_->binding->trackable_object;
  }

 private:

  Slot(details::Token *token = nullptr)
      : token_(token), skip_(false) {}

  ~Slot() {}

  details::Token *token_;
  bool skip_;

};

/**
 * @brief The basic class for an object which provides slot methods
 */
class Trackable {

  friend struct details::Binding;
  friend struct details::Token;
  template<typename ... ParamTypes> friend
  class Signal;

 public:

  Trackable()
      : first_binding_(nullptr), last_binding_(nullptr) {
  }

  virtual ~Trackable();

  Trackable(const Trackable &orig) {}

  Trackable &operator=(const Trackable &orig) {
    return *this;
  }

  /**
   * @brief Break the connection to a signal by given slot
   *
   * In SLOT you should use this method instead of Disconnect() to support 'disconnect on emit'.
   *
   * This method only works once.
   */
  void Unbind(SLOT slot);

  /**
    * @brief Break the all connections to this object
    */
  void UnbindAll();

  /**
   * @brief Break the all connections to this object
   *
   * When call this in a SLOT, pass the slot parameter to avoid damaging the iterator pointer
   * used in Slot object.
   *
   * @note You do not need to pass the slot parameter when calling this method on another object.
   * (slot->object() != this)
   */
  void UnbindAll(SLOT slot);

  /**
    * @brief Break all connections to the given slot method of this object
    */
  template<typename T, typename ... ParamTypes>
  void UnbindAll(void (T::*method)(ParamTypes...));

  /**
   * @brief Break all connections to the given slot method of this object
   *
   * When call this in a SLOT, pass the slot parameter to avoid damaging the iterator pointer
   * used in Slot object.
   *
   * @note You do not need to pass the slot parameter when calling this method on another object.
   * (slot->object() != this)
   */
  template<typename T, typename ... ParamTypes>
  void UnbindAll(SLOT slot, void (T::*method)(ParamTypes...));

  /**
   * @brief Count connections to the given slot method
   */
  template<typename T, typename ... ParamTypes>
  std::size_t CountBindings(void (T::*method)(ParamTypes...)) const;

  /**
   * @brief Count all connections
   */
  std::size_t CountBindings() const;

 protected:

  virtual void AuditDestroyingToken(details::Token *token) {}

  void PushBackBinding(details::Binding *binding);

  void PushFrontBinding(details::Binding *binding);

  void InsertBinding(int index, details::Binding *binding);

  static inline void link(details::Token *token, details::Binding *binding) {
#ifdef DEBUG
    assert((nullptr == token->binding) && (nullptr == binding->token));
#endif

    token->binding = binding;
    binding->token = token;
  }

  static inline void push_front(Trackable *trackable,
                                details::Binding *binding) {
    trackable->PushBackBinding(binding);
  }

  static inline void push_back(Trackable *trackable,
                               details::Binding *binding) {
    trackable->PushBackBinding(binding);
  }

  static inline void insert(Trackable *trackable,
                            details::Binding *binding,
                            int index = 0) {
    trackable->InsertBinding(index, binding);
  }

  details::Binding *first_binding() const {
    return first_binding_;
  }

  details::Binding *last_binding() const {
    return last_binding_;
  }

 private:

  details::Binding *first_binding_;
  details::Binding *last_binding_;
};

template<typename T, typename ... ParamTypes>
void Trackable::UnbindAll(void (T::*method)(ParamTypes...)) {
  details::Binding *p = nullptr;
  details::Binding *tmp = nullptr;
  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;

  p = last_binding_;
  while (p) {
    tmp = p->previous;
    delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(p->token);
    if (delegate_token && (delegate_token->delegate().template Equal<T>((T *) this, method))) {
      delete delegate_token;
    }
    p = tmp;
  }
}

template<typename T, typename ... ParamTypes>
void Trackable::UnbindAll(SLOT slot, void (T::*method)(ParamTypes...)) {
  if (nullptr == slot) {
    UnbindAll(method);
    return;
  }

#ifdef DEBUG
  if (slot->token_->binding->trackable_object != this) {
    fprintf(stderr, "No need to pass \'slot\' parameter to a SLOT method in another object.\n");
  }
#endif
  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;
  details::Token *tmp = nullptr;
  while (slot->token_->binding->trackable_object == this) {
    delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(slot->token_);
    if (delegate_token &&
        (delegate_token->delegate().template Equal<T>((T *) this, method))) {
      tmp = slot->token_;
      slot->token_ = slot->token_->next;
      delete tmp;
      slot->skip_ = true;
      if (nullptr == slot->token_) break;
    } else {
      break;
    }
  }

  UnbindAll(method);
}

template<typename T, typename ... ParamTypes>
size_t Trackable::CountBindings(void (T::*method)(ParamTypes...)) const {
  std::size_t count = 0;
  details::DelegateToken<ParamTypes...> *delegate_token = nullptr;

  for (details::Binding *p = first_binding_; p; p = p->next) {
    delegate_token = dynamic_cast<details::DelegateToken<ParamTypes...> *>(p->token);
    if (delegate_token && (delegate_token->delegate().template Equal<T>((T *) this, method))) {
      count++;
    }
  }
  return count;
}

/**
 * @brief A template class which can emit signal(s)
 */
template<typename ... ParamTypes>
class Signal : public Trackable {

  friend class Trackable;

  Signal(const Signal &orig) = delete;
  Signal &operator=(const Signal &orig) = delete;

 public:

  Signal()
      : Trackable(),
        first_token_(nullptr),
        last_token_(nullptr) {
  }

  virtual ~Signal() {
    DisconnectAll();
  }

  /**
   * @brief Connect this signal to a slot method in a observer
   */
  template<typename T>
  void Connect(T *obj, void (T::*method)(ParamTypes..., SLOT), int index = -1);

  void Connect(Signal<ParamTypes...> &other, int index = -1);

  /**
   * @brief Disconnect all delegates to a method
   */
  template<typename T>
  void DisconnectAll(T *obj, void (T::*method)(ParamTypes..., SLOT));

  /**
 * @brief Disconnect all delegates to a method
 *
 * This is uses in a slot method
 */
  template<typename T>
  void DisconnectAll(SLOT slot, T *obj, void (T::*method)(ParamTypes..., SLOT));

  /**
   * @brief Disconnect all signals
   */
  void DisconnectAll(Signal<ParamTypes...> &other);

  /**
 * @brief Disconnect all signals
 */
  void DisconnectAll(SLOT slot, Signal<ParamTypes...> &other);

  /**
   * @brief Disconnect delegats to a method by given start position and counts
   * @tparam T
   * @param obj
   * @param method
   * @param start_pos
   * @param counts
   * @return A integer number of how many delegates are found and disconnected
   *
   * By the default parameters this disconnect the last delegate to a method.
   */
  template<typename T>
  int Disconnect(T *obj, void (T::*method)(ParamTypes..., SLOT), int start_pos = -1, int counts = 1);

  template<typename T>
  int Disconnect(SLOT slot, T *obj, void (T::*method)(ParamTypes..., SLOT), int start_pos = -1, int counts = 1);

  /**
   * @brief Disconnect connections to a signal by given start position and counts
   * @param other
   * @param start_pos
   * @param counts
   * @return A integer number of how many signals are found and disconnected
   *
   * By the default parameters this disconnect the last signal.
   */
  int Disconnect(Signal<ParamTypes...> &other, int start_pos = -1, int counts = 1);

  int Disconnect(SLOT slot, Signal<ParamTypes...> &other, int start_pos = -1, int counts = 1);

  /**
   * @brief Disconnect all
   */
  void DisconnectAll();

  /**
   * @brief Disconnect all
   */
  void DisconnectAll(SLOT slot);

  template<typename T>
  bool IsConnectedTo(T *obj, void (T::*method)(ParamTypes..., SLOT)) const;

  bool IsConnectedTo(const Signal<ParamTypes...> &other) const;

  bool IsConnectedTo(const Trackable *obj) const;

  template<typename T>
  std::size_t CountConnections(T *obj, void (T::*method)(ParamTypes..., SLOT)) const;

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
template<typename T>
void Signal<ParamTypes...>::Connect(T *obj, void (T::*method)(ParamTypes..., SLOT), int index) {
  details::Binding *downstream = new details::Binding;
  Delegate<void(ParamTypes..., SLOT)> d =
      Delegate<void(ParamTypes..., SLOT)>::template FromMethod<T>(obj, method);
  details::DelegateToken<ParamTypes..., SLOT> *token = new details::DelegateToken<
      ParamTypes..., SLOT>(d);

  link(token, downstream);
  InsertToken(index, token);
  push_back(obj, downstream);  // always push back binding, don't care about the position in observer
}

template<typename ... ParamTypes>
void Signal<ParamTypes...>::Connect(Signal<ParamTypes...> &other, int index) {
  details::SignalToken<ParamTypes...> *token = new details::SignalToken<ParamTypes...>(
      other);
  details::Binding *binding = new details::Binding;

  link(token, binding);
  InsertToken(index, token);
  push_back(&other, binding);  // always push back binding, don't care about the position in observer
}

template<typename ... ParamTypes>
template<typename T>
void Signal<ParamTypes...>::DisconnectAll(T *obj, void (T::*method)(ParamTypes..., SLOT)) {
  details::DelegateToken<ParamTypes..., SLOT> *delegate_token = nullptr;
  details::Token *p = last_token_;
  details::Token *tmp = nullptr;

  while (p) {
    tmp = p->previous;
    if (p->binding->trackable_object == obj) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes..., SLOT> * > (p);
      if (delegate_token && (delegate_token->delegate().template Equal<T>(obj, method))) {
        delete delegate_token;
      }
    }
    p = tmp;
  }
}

template<typename ... ParamTypes>
template<typename T>
void Signal<ParamTypes...>::DisconnectAll(SLOT slot, T *obj, void (T::*method)(ParamTypes..., SLOT)) {
  if ((nullptr == slot) || (slot->token_->trackable_object != this)) {
    DisconnectAll(obj, method);
    return;
  }

  // This signal is emitting, a void impact the iterator in the slot object

  details::DelegateToken<ParamTypes..., SLOT> *delegate_token = nullptr;
  details::Token *p = last_token_;
  details::Token *tmp = nullptr;

  bool found = false;
  details::Token *next = nullptr;

  while (p) {
    tmp = p->previous;
    if (p == slot->token_) {
      next = p->next;
      found = true;
    }
    if (p->binding->trackable_object == obj) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes..., SLOT> * > (p);
      if (delegate_token && (delegate_token->delegate().template Equal<T>(obj, method))) {
        delete delegate_token;
      }
    }
    p = tmp;
  }
  if (found) {
    slot->skip_ = true;
    slot->token_ = next;
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
void Signal<ParamTypes...>::DisconnectAll(SLOT slot, Signal<ParamTypes...> &other) {
  if ((nullptr == slot) || (slot->token_->trackable_object != this)) {
    DisconnectAll(other);
    return;
  }

  details::SignalToken<ParamTypes...> *signal_token = nullptr;
  details::Token *p = last_token_;
  details::Token *tmp = nullptr;

  bool found = false;
  details::Token *next = nullptr;

  while (p) {
    tmp = p->previous;
    if (p == slot->token_) {
      next = p->next;
      found = true;
    }
    if (p->binding->trackable_object == (&other)) {
      signal_token = dynamic_cast<details::SignalToken<ParamTypes...> * > (p);
      if (signal_token && (signal_token->signal() == (&other))) {
        delete signal_token;
      }
    }
    p = tmp;
  }
  if (found) {
    slot->skip_ = true;
    slot->token_ = next;
  }
}

template<typename ... ParamTypes>
template<typename T>
int Signal<ParamTypes...>::Disconnect(T *obj, void (T::*method)(ParamTypes..., SLOT), int start_pos, int counts) {
  details::DelegateToken<ParamTypes..., SLOT> *delegate_token = nullptr;
  details::Token *p = nullptr;
  details::Token *tmp = nullptr;
  int ret_count = 0;

  if (start_pos >= 0) {
    p = first_token_;
    while (p && (start_pos > 0)) {
      p = p->next;
      start_pos--;
    }

    while (p) {
      tmp = p->next;
      if (p->binding->trackable_object == obj) {
        delegate_token = dynamic_cast<details::DelegateToken<ParamTypes..., SLOT> * > (p);
        if (delegate_token && (delegate_token->delegate().template Equal<T>(obj, method))) {
          ret_count++;
          counts--;
          delete p;
        }
      }
      if (counts == 0) break;
      p = tmp;
    }
  } else {
    p = last_token_;
    while (p && (start_pos < -1)) {
      p = p->previous;
      start_pos++;
    }

    while (p) {
      tmp = p->previous;
      if (p->binding->trackable_object == obj) {
        delegate_token = dynamic_cast<details::DelegateToken<ParamTypes..., SLOT> * > (p);
        if (delegate_token && (delegate_token->delegate().template Equal<T>(obj, method))) {
          ret_count++;
          counts--;
          delete p;
        }
      }
      if (counts == 0) break;
      p = tmp;
    }
  }

  return ret_count;
}

template<typename ... ParamTypes>
template<typename T>
int Signal<ParamTypes...>::Disconnect(SLOT slot,
                                      T *obj,
                                      void (T::*method)(ParamTypes..., SLOT),
                                      int start_pos,
                                      int counts) {
  if ((nullptr == slot) || (slot->token_->trackable_object != this)) {
    return Disconnect(obj, method, start_pos, counts);
  }

  details::DelegateToken<ParamTypes..., SLOT> *delegate_token = nullptr;
  details::Token *p = nullptr;
  details::Token *tmp = nullptr;
  int ret_count = 0;

  bool found = false;
  details::Token *next = nullptr;

  if (start_pos >= 0) {
    p = first_token_;
    while (p && (start_pos > 0)) {
      p = p->next;
      start_pos--;
    }

    while (p) {
      tmp = p->next;
      if (p == slot->token_) {
        found = true;
      }
      if (p->binding->trackable_object == obj) {
        delegate_token = dynamic_cast<details::DelegateToken<ParamTypes..., SLOT> * > (p);
        if (delegate_token && (delegate_token->delegate().template Equal<T>(obj, method))) {
          ret_count++;
          counts--;
          delete p;
          next = tmp;
        }
      }
      if (counts == 0) break;
      p = tmp;
    }
    if (found) {
      slot->token_ = next;
      slot->skip_ = true;
    }
  } else {
    p = last_token_;
    while (p && (start_pos < -1)) {
      p = p->previous;
      start_pos++;
    }

    while (p) {
      tmp = p->previous;
      if (p == slot->token_) {
        next = p->next;
        found = true;
      }
      if (p->binding->trackable_object == obj) {
        delegate_token = dynamic_cast<details::DelegateToken<ParamTypes..., SLOT> * > (p);
        if (delegate_token && (delegate_token->delegate().template Equal<T>(obj, method))) {
          ret_count++;
          counts--;
          delete p;
        }
      }
      if (counts == 0) break;
      p = tmp;
    }
    if (found) {
      slot->token_ = next;
      slot->skip_ = true;
    }
  }

  return ret_count;
}

template<typename ... ParamTypes>
int Signal<ParamTypes...>::Disconnect(Signal<ParamTypes...> &other, int start_pos, int counts) {
  details::SignalToken<ParamTypes...> *signal_token = nullptr;
  details::Token *p = nullptr;
  details::Token *tmp = nullptr;
  int ret_count = 0;

  if (start_pos >= 0) {
    p = first_token_;
    while (p && (start_pos > 0)) {
      p = p->next;
      start_pos--;
    }

    while (p) {
      tmp = p->next;
      if (p->binding->trackable_object == (&other)) {
        signal_token = dynamic_cast<details::SignalToken<ParamTypes...> * > (p);
        if (signal_token && (signal_token->signal() == (&other))) {
          ret_count++;
          counts--;
          delete p;
        }
      }
      if (counts == 0) break;
      p = tmp;
    }

  } else {
    p = last_token_;
    while (p && (start_pos < -1)) {
      p = p->previous;
      start_pos++;
    }

    while (p) {
      tmp = p->previous;
      if (p->binding->trackable_object == (&other)) {
        signal_token = dynamic_cast<details::SignalToken<ParamTypes...> * > (p);
        if (signal_token && (signal_token->signal() == (&other))) {
          ret_count++;
          counts--;
          delete p;
        }
      }
      if (counts == 0) break;
      p = tmp;
    }

  }

  return ret_count;
}

template<typename ... ParamTypes>
int Signal<ParamTypes...>::Disconnect(SLOT slot, Signal<ParamTypes...> &other, int start_pos, int counts) {
  if ((nullptr == slot) || (slot->token_->trackable_object != this)) {
    return Disconnect(other, start_pos, counts);
  }

  details::SignalToken<ParamTypes...> *signal_token = nullptr;
  details::Token *p = nullptr;
  details::Token *tmp = nullptr;
  int ret_count = 0;

  bool found = false;
  details::Token *next = nullptr;

  if (start_pos >= 0) {
    p = first_token_;
    while (p && (start_pos > 0)) {
      p = p->next;
      start_pos--;
    }

    while (p) {
      tmp = p->next;
      if (p == slot->token_) {
        found = true;
      }
      if (p->binding->trackable_object == (&other)) {
        signal_token = dynamic_cast<details::SignalToken<ParamTypes...> * > (p);
        if (signal_token && (signal_token->signal() == (&other))) {
          ret_count++;
          counts--;
          delete p;
          next = tmp;
        }
      }
      if (counts == 0) break;
      p = tmp;
    }
    if (found) {
      slot->token_ = next;
      slot->skip_ = true;
    }
  } else {
    p = last_token_;
    while (p && (start_pos < -1)) {
      p = p->previous;
      start_pos++;
    }

    while (p) {
      tmp = p->previous;
      if (p == slot->token_) {
        next = p->next;
        found = true;
      }
      if (p->binding->trackable_object == (&other)) {
        signal_token = dynamic_cast<details::SignalToken<ParamTypes...> * > (p);
        if (signal_token && (signal_token->signal() == (&other))) {
          ret_count++;
          counts--;
          delete p;
        }
      }
      if (counts == 0) break;
      p = tmp;
    }
    if (found) {
      slot->token_ = next;
      slot->skip_ = true;
    }
  }

  return ret_count;
}

template<typename ... ParamTypes>
template<typename T>
bool Signal<ParamTypes...>::IsConnectedTo(T *obj, void (T::*method)(ParamTypes..., SLOT)) const {
  details::DelegateToken<ParamTypes..., SLOT> *delegate_token = nullptr;

  for (details::Token *p = first_token_; p; p = p->next) {
    if (p->binding->trackable_object == obj) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes..., SLOT> *>(p);
      if (delegate_token && (delegate_token->delegate().template Equal<T>(obj, method))) {
        return true;
      }
    }
  }
  return false;
}

template<typename ... ParamTypes>
bool Signal<ParamTypes...>::IsConnectedTo(const Signal<ParamTypes...> &other) const {
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
bool Signal<ParamTypes...>::IsConnectedTo(const Trackable *obj) const {
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
std::size_t Signal<ParamTypes...>::CountConnections(T *obj, void (T::*method)(ParamTypes..., SLOT)) const {
  std::size_t count = 0;
  details::DelegateToken<ParamTypes..., SLOT> *delegate_token = nullptr;

  for (details::Token *p = first_token_; p; p = p->next) {
    if (p->binding->trackable_object == obj) {
      delegate_token = dynamic_cast<details::DelegateToken<ParamTypes..., SLOT> *>(p);
      if (delegate_token && (delegate_token->delegate().template Equal<T>(obj, method))) {
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
    static_cast<details::CallableToken<ParamTypes..., SLOT> * > (slot.token_)->Invoke(Args..., &slot);

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
  assert(nullptr == token->trackable_object);
#endif

  if (last_token_) {
    last_token_->next = token;
    token->previous = last_token_;
  } else {
#ifdef DEBUG
    assert(nullptr == first_token_);
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
  assert(nullptr == token->trackable_object);
#endif

  if (first_token_) {
    first_token_->previous = token;
    token->next = first_token_;
  } else {
#ifdef DEBUG
    assert(nullptr == last_token_);
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
  assert(nullptr == token->trackable_object);
#endif

  if (nullptr == first_token_) {
#ifdef DEBUG
    assert(nullptr == last_token_);
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

template<typename ... ParamTypes>
void Signal<ParamTypes...>::DisconnectAll(SLOT slot) {
  if ((nullptr == slot) || (slot->token_->trackable_object != this)) {
    DisconnectAll();
    return;
  }

  details::Token *tmp = nullptr;
  details::Token *p = first_token_;

  while (p) {
    tmp = p->next;
    delete p;
    p = tmp;
  }

  slot->token_ = nullptr;
  slot->skip_ = true;
}

/**
 * @brief A reference to a corresponding signal
 */
template<typename ... ParamTypes>
class SignalRef {

  SignalRef() = delete;

 public:

  SignalRef(Signal<ParamTypes...> &signal)
      : signal_(&signal) {
  }

  SignalRef(const SignalRef &orig)
      : signal_(orig.signal_) {
  }

  ~SignalRef() {}

  SignalRef &operator=(const SignalRef &other) {
    signal_ = other.signal_;
    return *this;
  }

  template<typename T>
  void Connect(T *obj, void (T::*method)(ParamTypes..., SLOT), int index = -1) {
    signal_->Connect(obj, method, index);
  }

  void Connect(Signal<ParamTypes...> &signal, int index = -1) {
    signal_->Connect(signal, index);
  }

  template<typename T>
  void DisconnectAll(T *obj, void (T::*method)(ParamTypes..., SLOT)) {
    signal_->DisconnectAll(obj, method);
  }

  template<typename T>
  void DisconnectAll(SLOT slot, T *obj, void (T::*method)(ParamTypes..., SLOT)) {
    signal_->DisconnectAll(slot, obj, method);
  }

  void DisconnectAll(Signal<ParamTypes...> &signal) {
    signal_->DisconnectAll(signal);
  }

  void DisconnectAll(SLOT slot, Signal<ParamTypes...> &signal) {
    signal_->DisconnectAll(slot, signal);
  }

  template<typename T>
  int Disconnect(T *obj, void (T::*method)(ParamTypes..., SLOT), int start_pos = -1, int counts = 1) {
    return signal_->Disconnect(obj, method, start_pos, counts);
  }

  template<typename T>
  int Disconnect(SLOT slot, T *obj, void (T::*method)(ParamTypes..., SLOT), int start_pos = -1, int counts = 1) {
    return signal_->Disconnect(slot, obj, method, start_pos, counts);
  }

  int Disconnect(Signal<ParamTypes...> &signal, int start_pos = -1, int counts = 1) {
    return signal_->Disconnect(signal, start_pos, counts);
  }

  int Disconnect(SLOT slot, Signal<ParamTypes...> &signal, int start_pos = -1, int counts = 1) {
    return signal_->Disconnect(slot, signal, start_pos, counts);
  }

  void DisconnectAll() {
    signal_->DisconnectAll();
  }

  void DisconnectAll(SLOT slot) {
    signal_->DisconnectAll(slot);
  }

  template<typename T>
  bool IsConnectedTo(T *obj, void (T::*method)(ParamTypes..., SLOT)) const {
    return signal_->IsConnectedTo(obj, method);
  }

  bool IsConnectedTo(const Signal<ParamTypes...> &signal) const {
    return signal_->IsConnectedTo(signal);
  }

  bool IsConnectedTo(const Trackable *obj) const {
    return signal_->IsConnectedTo(obj);
  }

  template<typename T>
  std::size_t CountConnections(T *obj, void (T::*method)(ParamTypes..., SLOT)) const {
    return signal_->CountConnections(obj, method);
  }

  std::size_t CountConnections(const Signal<ParamTypes...> &signal) const {
    return signal_->CountConnections(signal);
  }

  std::size_t CountConnections() const {
    return signal_->CountConnections();
  }

  std::size_t CountBindings() const {
    return signal_->CountBindings();
  }

 private:

  Signal<ParamTypes...> *signal_;

};

} // namespace sigcxx

#endif  // SIGCXX_SIGCXX_HPP_

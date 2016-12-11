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

#ifndef SIGCXX_DELEGATE_HPP_
#define SIGCXX_DELEGATE_HPP_

#include <cstring>

namespace sigcxx {

/// @cond IGNORE

// generic classes to calculate method pointer:
class GenericBase1 {};
class GenericBase2 {};
class GenericMultiInherit : GenericBase1, GenericBase2 {};

typedef void (GenericMultiInherit::*GenericMethodPointer)();

// A forward declaration
// This is the key to use the template format of Delegate<return_type (args...)>
template<typename _Signature>
class Delegate;

template<typename _Signature>
class DelegateRef;

/// @endcond

/**
 * @brief Delegate with variadic template
 *
 * Inspired by http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
 *
 * @note only support method in this project, which means you cannot
 * create a delegate to a static member function
 */
template<typename ReturnType, typename ... ParamTypes>
class Delegate<ReturnType(ParamTypes...)> {
  typedef ReturnType (*MethodStubType)(void *object_ptr, GenericMethodPointer, ParamTypes...);

  struct PointerData {

    PointerData()
        : object_pointer(nullptr),
          method_stub(nullptr),
          method_pointer(nullptr) {}

    PointerData(const PointerData &orig)
        : object_pointer(orig.object_pointer),
          method_stub(orig.method_stub),
          method_pointer(orig.method_pointer) {}

    PointerData &operator=(const PointerData &orig) {
      object_pointer = orig.object_pointer;
      method_stub = orig.method_stub;
      method_pointer = orig.method_pointer;
      return *this;
    }

    void *object_pointer;
    MethodStubType method_stub;
    GenericMethodPointer method_pointer;
  };

  template<typename T, typename TFxn>
  struct MethodStub {
    static ReturnType invoke(void *obj_ptr, GenericMethodPointer any, ParamTypes ... Args) {
      T *obj = static_cast<T *>(obj_ptr);
      return (obj->*reinterpret_cast<TFxn>(any))(Args...);
    }
  };

 public:

  /**
   * @brief Create a delegate from the given object and a member function pointer
   * @tparam T The object type
   * @param object_ptr A pointer to an object
   * @param method A pointer to a member function in class T
   * @return A delegate object
   */
  template<typename T>
  static inline Delegate FromMethod(T *object_ptr,
                                    ReturnType (T::*method)(ParamTypes...)) {
    typedef ReturnType (T::*TMethod)(ParamTypes...);

    Delegate d;
    d.data_.object_pointer = object_ptr;
    d.data_.method_stub = &MethodStub<T, TMethod>::invoke;
    d.data_.method_pointer = reinterpret_cast<GenericMethodPointer>(method);

    return d;
  }

  /**
   * @brief Create a delegate from the given object and a const member function pointer
   * @tparam T The object type
   * @param object_ptr A pointer to an object
   * @param method A pointer to a member function in class T
   * @return A delegate object
   */
  template<typename T>
  static inline Delegate FromMethod(T *object_ptr,
                                    ReturnType (T::*method)(ParamTypes...) const) {
    typedef ReturnType (T::*TMethod)(ParamTypes...) const;

    Delegate d;
    d.data_.object_pointer = object_ptr;
    d.data_.method_stub = &MethodStub<T, TMethod>::invoke;
    d.data_.method_pointer = reinterpret_cast<GenericMethodPointer>(method);

    return d;
  }

  Delegate() {}

  template<typename T>
  Delegate(T *object_ptr, ReturnType (T::*method)(ParamTypes...)) {
    typedef ReturnType (T::*TMethod)(ParamTypes...);

    data_.object_pointer = object_ptr;
    data_.method_stub = &MethodStub<T, TMethod>::invoke;
    data_.method_pointer = reinterpret_cast<GenericMethodPointer>(method);
  }

  template<typename T>
  Delegate(T *object_ptr, ReturnType (T::*method)(ParamTypes...) const) {
    typedef ReturnType (T::*TMethod)(ParamTypes...) const;

    data_.object_pointer = object_ptr;
    data_.method_stub = &MethodStub<T, TMethod>::invoke;
    data_.method_pointer = reinterpret_cast<GenericMethodPointer>(method);
  }

  Delegate(const Delegate &orig)
      : data_(orig.data_) {}

  ~Delegate() {}

  void Reset() {
    memset(&data_, 0, sizeof(PointerData));
  }

  Delegate &operator=(const Delegate &orig) {
    data_ = orig.data_;
    return *this;
  }

  ReturnType operator()(ParamTypes... Args) const {
    return (*data_.method_stub)(data_.object_pointer, data_.method_pointer, Args...);
  }

  ReturnType Invoke(ParamTypes... Args) const {
    return (*data_.method_stub)(data_.object_pointer, data_.method_pointer, Args...);
  }

  operator bool() const {
    // Support method delegate only, no need to check other members:
    return data_.method_pointer != nullptr;
  }

  template<typename T>
  bool Equal(T *object_ptr, ReturnType(T::*method)(ParamTypes...)) const {
    typedef ReturnType (T::*TMethod)(ParamTypes...);

    return (data_.object_pointer == object_ptr) &&
        (data_.method_stub == &MethodStub<T, TMethod>::invoke) &&
        (data_.method_pointer == reinterpret_cast<GenericMethodPointer>(method));
  }

  template<typename T>
  bool Equal(T *object_ptr, ReturnType(T::*method)(ParamTypes...) const) const {
    typedef ReturnType (T::*TMethod)(ParamTypes...) const;

    return (data_.object_pointer == object_ptr) &&
        (data_.method_stub == &MethodStub<T, TMethod>::invoke) &&
        (data_.method_pointer == reinterpret_cast<GenericMethodPointer>(method));
  }

 private:

  template<typename ReturnTypeAlias, typename ... ParamTypesAlias>
  friend inline bool operator==(const Delegate<ReturnTypeAlias(ParamTypesAlias...)> &src,
                                const Delegate<ReturnTypeAlias(ParamTypesAlias...)> &dst);

  template<typename ReturnTypeAlias, typename ... ParamTypesAlias>
  friend inline bool operator!=(const Delegate<ReturnTypeAlias(ParamTypesAlias...)> &src,
                                const Delegate<ReturnTypeAlias(ParamTypesAlias...)> &dst);

  template<typename ReturnTypeAlias, typename ... ParamTypesAlias>
  friend inline bool operator<(const Delegate<ReturnTypeAlias(ParamTypesAlias...)> &src,
                               const Delegate<ReturnTypeAlias(ParamTypesAlias...)> &dst);

  template<typename ReturnTypeAlias, typename ... ParamTypesAlias>
  friend inline bool operator>(const Delegate<ReturnTypeAlias(ParamTypesAlias...)> &src,
                               const Delegate<ReturnTypeAlias(ParamTypesAlias...)> &dst);

  PointerData data_;
};

template<typename ReturnType, typename ... ParamTypes>
inline bool operator==(const Delegate<ReturnType(ParamTypes...)> &src,
                       const Delegate<ReturnType(ParamTypes...)> &dst) {
  return memcmp(&src.data_, &dst.data_,
                sizeof(typename Delegate<ReturnType(ParamTypes...)>::PointerData)
  ) == 0;
}

template<typename ReturnType, typename ... ParamTypes>
inline bool operator!=(const Delegate<ReturnType(ParamTypes...)> &src,
                       const Delegate<ReturnType(ParamTypes...)> &dst) {
  return memcmp(&src.data_, &dst.data_,
                sizeof(typename Delegate<ReturnType(ParamTypes...)>::PointerData)
  ) != 0;
}

template<typename ReturnType, typename ... ParamTypes>
inline bool operator<(const Delegate<ReturnType(ParamTypes...)> &src,
                      const Delegate<ReturnType(ParamTypes...)> &dst) {
  return memcmp(&src.data_, &dst.data_,
                sizeof(typename Delegate<ReturnType(ParamTypes...)>::PointerData)
  ) < 0;
}

template<typename ReturnType, typename ... ParamTypes>
inline bool operator>(const Delegate<ReturnType(ParamTypes...)> &src,
                      const Delegate<ReturnType(ParamTypes...)> &dst) {
  return memcmp(&src.data_, &dst.data_,
                sizeof(typename Delegate<ReturnType(ParamTypes...)>::PointerData)
  ) > 0;
}

/**
 * @brief A reference to expose delegate in an object
 */
template<typename ReturnType, typename ... ParamTypes>
class DelegateRef<ReturnType(ParamTypes...)> {
 public:

  DelegateRef() = delete;

  DelegateRef(Delegate<ReturnType(ParamTypes...)> &delegate)
      : delegate_(&delegate) {}

  DelegateRef(const DelegateRef &orig)
      : delegate_(orig.delegate_) {}

  ~DelegateRef() {}

  DelegateRef &operator=(const DelegateRef &orig) {
    delegate_ = orig.delegate_;
    return *this;
  }

  template<typename T>
  void Set(T *obj, ReturnType (T::*method)(ParamTypes...)) {
    *delegate_ = Delegate<ReturnType(ParamTypes...)>::template FromMethod<T>(obj, method);
  }

  template<typename T>
  void Set(T *obj, ReturnType (T::*method)(ParamTypes...) const) {
    *delegate_ = Delegate<ReturnType(ParamTypes...)>::template FromMethod<T>(obj, method);
  }

  void Reset() {
    delegate_->Reset();
  }

  template<typename T>
  bool IsAssignedTo(T *obj, ReturnType (T::*method)(ParamTypes...)) {
    return delegate_->Equal(obj, method);
  }

  template<typename T>
  bool IsAssignedTo(T *obj, ReturnType (T::*method)(ParamTypes...) const) {
    return delegate_->Equal(obj, method);
  }

  operator bool() const {
    return (*delegate_);
  }

 private:
  Delegate<ReturnType(ParamTypes...)> *delegate_;
};

} // namespace sigcxx

#endif  // SIGCXX_DELEGATE_HPP_

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

namespace CppEvent {

/**
 * @brief Delegate with variadic template
 *
 * Inspired by http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
 *
 * @note only support method in this project, which means you cannot
 * create a delegate to a static member function
 */
template<typename ReturnType, typename ... ParamTypes>
class Delegate
{
  typedef ReturnType (*MethodType) (void* object_ptr, ParamTypes...);

public:

  template<typename T, ReturnType (T::*TMethod) (ParamTypes...)>
  static inline Delegate from_method (T* object_ptr)
  {
    return Delegate(object_ptr, &method_stub<T, TMethod>);
  }

  template<typename T, ReturnType (T::*TMethod) (ParamTypes...) const>
  static inline Delegate from_const_method (T* object_ptr)
  {
    return Delegate(object_ptr, &const_method_stub<T, TMethod>);
  }

  inline Delegate ()
  : object_ptr_(0), method_ptr_(0)
  { }

  inline Delegate (void* object_ptr, MethodType method_ptr)
  : object_ptr_(object_ptr), method_ptr_(method_ptr)
  { }

  inline Delegate (const Delegate& orig)
  : object_ptr_(orig.object_ptr_), method_ptr_(orig.method_ptr_)
  {
  }

  inline ~Delegate () {}

  inline void reset ()
  {
    object_ptr_ = 0;
    method_ptr_ = 0;
  }

  inline Delegate& operator = (const Delegate& orig)
  {
    object_ptr_ = orig.object_ptr_;
    method_ptr_ = orig.method_ptr_;
    return *this;
  }

  inline ReturnType operator () (ParamTypes... Args) const
  {
    return (*method_ptr_)(object_ptr_, Args...);
  }

  /*
  inline ReturnType invoke(ParamTypes... Args) const
  {
    return (*method_ptr)(object_ptr_, Args...);
  }
  */

  inline operator bool () const
  {
    return (object_ptr_ != 0) && (method_ptr_ != 0);
  }

  inline bool operator == (const Delegate& other) const
  {
    return (other.object_ptr_ == object_ptr_)
        && (other.method_ptr_ == method_ptr_);
  }

  inline bool operator != (const Delegate& other) const
  {
    return (other.object_ptr_ != object_ptr_)
        || (other.method_ptr_ != method_ptr_);
  }

  template<typename T, ReturnType(T::*TMethod) (ParamTypes...)>
  inline bool equal(T* object_ptr) const
  {
    return (object_ptr_ == object_ptr) &&
      (method_ptr_ == &method_stub<T, TMethod>);
  }

  template<typename T, ReturnType(T::*TMethod) (ParamTypes...) const>
  inline bool equal(T* object_ptr) const
  {
    return (object_ptr_ == object_ptr) &&
      (method_ptr_ == &const_method_stub<T, TMethod>);
  }

  inline void* object_ptr() const
  {
    return object_ptr_;
  }

  inline MethodType method_ptr () const
  {
    return method_ptr_;
  }

private:

  template<typename ReturnTypeAlias, typename ... ParamTypesAlias>
  friend inline bool operator == (const Delegate<ReturnTypeAlias,
                                      ParamTypesAlias...>& src,
                                  const Delegate<ReturnTypeAlias,
                                      ParamTypesAlias...>& dst);

  template<typename ReturnTypeAlias, typename ... ParamTypesAlias>
  friend inline bool operator < (const Delegate<ReturnTypeAlias,
    ParamTypesAlias...>& src,
    const Delegate<ReturnTypeAlias,
    ParamTypesAlias...>& dst);

  template<typename ReturnTypeAlias, typename ... ParamTypesAlias>
  friend inline bool operator > (const Delegate<ReturnTypeAlias,
    ParamTypesAlias...>& src,
    const Delegate<ReturnTypeAlias,
    ParamTypesAlias...>& dst);

  template<typename T, ReturnType (T::*TMethod) (ParamTypes...)>
  static inline void method_stub (void* object_ptr, ParamTypes... Args)
  {
    T* p = static_cast<T*>(object_ptr);
    return (p->*TMethod)(Args...);
  }

  template<typename T, ReturnType (T::*TMethod) (ParamTypes...) const>
  static inline void const_method_stub (void* object_ptr, ParamTypes... Args)
  {
    T* p = static_cast<T*>(object_ptr);
    return (p->*TMethod)(Args...);
  }

  void* object_ptr_;
  MethodType method_ptr_;
};

template<typename ReturnType, typename ... ParamTypes>
inline bool operator == (const Delegate<ReturnType, ParamTypes...>& src,
                         const Delegate<ReturnType, ParamTypes...>& dst)
{
  return (src.object_ptr_ == dst.object_ptr_)
      && (src.method_ptr_ == dst.method_ptr_);
}

template<typename ReturnType, typename ... ParamTypes>
inline bool operator < (const Delegate<ReturnType, ParamTypes...>& src,
  const Delegate<ReturnType, ParamTypes...>& dst)
{
  if (src.object_ptr_ != dst.object_ptr_) {
    return src.object_ptr_ < dst.object_ptr_;
  }
  else {
    return src.method_ptr_ < dst.method_ptr_;
  }
}

template<typename ReturnType, typename ... ParamTypes>
inline bool operator > (const Delegate<ReturnType, ParamTypes...>& src,
  const Delegate<ReturnType, ParamTypes...>& dst)
{
  if (src.object_ptr_ != dst.object_ptr_) {
    return src.object_ptr_ > dst.object_ptr_;
  }
  else {
    return src.method_ptr_ > dst.method_ptr_;
  }
}

} // namespace CppEvent

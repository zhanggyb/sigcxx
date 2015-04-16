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

// generic classes to calculate method pointer:
class GenericBase1 {};
class GenericBase2 {};
class GenericMultiInherit: GenericBase1, GenericBase2 {};

typedef void (GenericMultiInherit::*GenericMethodPointer)();

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
  typedef ReturnType (*MethodPointer) (void* object_ptr, GenericMethodPointer, ParamTypes...);

  template<typename T, typename TFxn>
  struct MethodStub
  {
    static void invoke (void* obj_ptr, GenericMethodPointer any, ParamTypes ... Args)
    {
      T* obj = static_cast<T*>(obj_ptr);
      (obj->*reinterpret_cast<TFxn>(any))(Args...);
    }
  };

  template<typename T, typename TFxn>
  struct ConstMethodStub
  {
    static void invoke (void* obj_ptr, GenericMethodPointer any, ParamTypes ... Args)
    {
      T* obj = static_cast<T*>(obj_ptr);
      (obj->*reinterpret_cast<TFxn>(any))(Args...);
    }
  };
  
 public:

  template<typename T>
  static inline Delegate from_method (T* object_ptr,
                                      ReturnType (T::*method) (ParamTypes...))
  {
    typedef ReturnType (T::*TMethod)(ParamTypes...);

    Delegate d;
    d.object_ptr_ = object_ptr;
    d.any_ptr_ = reinterpret_cast<GenericMethodPointer>(method);
    d.method_ptr_ = &MethodStub<T, TMethod>::invoke;

    return d;
  }
  
  inline Delegate ()
      : object_ptr_(0), any_ptr_(0), method_ptr_(0)
  { }

  inline Delegate (const Delegate& orig)
      : object_ptr_(orig.object_ptr_),
        any_ptr_(orig.any_ptr_),
        method_ptr_(orig.method_ptr_)
  { }

  inline ~Delegate () {}

  inline void reset ()
  {
    object_ptr_ = 0;
    any_ptr_ = 0;
    method_ptr_ = 0;
  }
  
  inline Delegate& operator = (const Delegate& orig)
  {
    object_ptr_ = orig.object_ptr_;
    any_ptr_ = orig.any_ptr_;
    method_ptr_ = orig.method_ptr_;
    return *this;
  }
  
  inline ReturnType operator () (ParamTypes... Args) const
  {
    return (*method_ptr_)(object_ptr_, any_ptr_, Args...);
  }
  
  /*
    inline ReturnType invoke(ParamTypes... Args) const
    {
    return (*method_ptr)(object_ptr_, Args...);
    }
  */
  
  inline operator bool () const
  {
    return (object_ptr_ != 0) &&
        (any_ptr_ != 0) &&
        (method_ptr_ != 0);
  }
  
  inline bool operator == (const Delegate& other) const
  {
    return (other.object_ptr_ == object_ptr_)
        && (other.any_ptr_ == any_ptr_)
        && (other.method_ptr_ == method_ptr_);
  }
  
  inline bool operator != (const Delegate& other) const
  {
    return (other.object_ptr_ != object_ptr_)
        || (other.any_ptr_ != any_ptr_)
        || (other.method_ptr_ != method_ptr_);
  }
  
  template<typename T>
  inline bool equal(T* object_ptr, ReturnType(T::*method) (ParamTypes...)) const
  {
    typedef ReturnType (T::*TMethod)(ParamTypes...);
    
    return (object_ptr_ == object_ptr)
        && (any_ptr_ == reinterpret_cast<GenericMethodPointer>(method))
        && (method_ptr_ == &MethodStub<T, TMethod>::invoke);
  }
  
  template<typename T>
  inline bool equal(T* object_ptr, ReturnType(T::*method) (ParamTypes...) const) const
  {
    typedef ReturnType (T::*TMethod)(ParamTypes...) const;
    
    return (object_ptr_ == object_ptr) &&
        (any_ptr_ == reinterpret_cast<GenericMethodPointer>(method)) &&
        (method_ptr_ == &MethodStub<T, TMethod>::invoke);
  }

  inline void* object_ptr() const
  {
    return object_ptr_;
  }

  inline GenericMethodPointer any_ptr() const
  {
    return any_ptr_;
  }
  
  inline MethodPointer method_ptr () const
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

  void* object_ptr_;
  GenericMethodPointer any_ptr_;
  MethodPointer method_ptr_;
};

template<typename ReturnType, typename ... ParamTypes>
inline bool operator == (const Delegate<ReturnType, ParamTypes...>& src,
                         const Delegate<ReturnType, ParamTypes...>& dst)
{
  return (src.object_ptr_ == dst.object_ptr_)
      && (src.any_ptr_ == dst.any_ptr_)
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

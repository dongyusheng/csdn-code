// SmartPtr.cpp

#include "SmartPtr.h"

template<typename T>
SmartPtr<T>::SmartPtr(T *p)
{
    // 与p指向同一块内存, 并将引用计数初始化为1
    _mPtr = p;
    _mpRef = new size_t(1);
}

template<typename T>
SmartPtr<T>::SmartPtr(SmartPtr<T>& other)
{
    _mPtr = other._mPtr;
    other._mpRef++;
    _mpRef = other._mpRef;
}

template<typename T>
SmartPtr<T>::~SmartPtr()
{
    decrRef();
}

template<typename T>
SmartPtr<T>& SmartPtr<T>::operator=(SmartPtr<T>& rhs)
{
    // 将自身的引用计数减1
    decrRef();

    // 指向于参数所指的对象, 并将参数所指对象的引用计数+1, 并指向其引用计数
    _mPtr = rhs._mPtr;
    rhs._mpRef++;
    _mpRef = rhs._mpRef;
    
    return *this;
}

template<typename T>
T& SmartPtr<T>::operator*()
{
    if(_mPtr)
        return *_mPtr;

    throw std::runtime_error("dereference of  NULL pointer");
}

template<typename T>
T* SmartPtr<T>::operator->()
{
    if(_mPtr)
        return _mPtr;

    throw std::runtime_error("access rgeough NULL pointer");
}

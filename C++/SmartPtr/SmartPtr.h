// SmartPtr.h
#ifndef __SMARTPTR_H_
#define __SMARTPTR_H_

#include <iostream>

using namespace std;

template<typename T>
class SmartPtr
{
public:
    SmartPtr(T *p = 0);         // 构造函数
    SmartPtr(SmartPtr<T>& other);  // 拷贝构造函数
    ~SmartPtr();                // 析构函数

    SmartPtr& operator=(SmartPtr<T>& rhs); // 赋值运算符

    T& operator*();
    T* operator->();
private:
    // 将对象的引用计数减1
    void decrRef()
    {
        // 如果引用计数减1之后变为0, 则释放该智能指针的底层数据, 此智能指针不再继续使用
        if(--*_mpRef == 0)
        {
            delete _mPtr;
            delete _mpRef;
        }
    }
private:
    T *_mPtr;       // 保存数据对象指针
    size_t *_mpRef; // 当前数据对象的引用计数
};

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

#endif // __SMARTPTR_H_

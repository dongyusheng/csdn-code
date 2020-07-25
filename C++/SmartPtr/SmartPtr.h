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

#endif // __SMARTPTR_H_

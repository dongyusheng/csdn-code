/*
 * @Author: your name
 * @Date: 2020-02-14 19:41:58
 * @LastEditTime : 2020-02-14 19:48:40
 * @LastEditors  : Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \im\pack_test.cpp
 */
#include <iostream>
#include <stdio.h>

#pragma pack(4)  
class CBase4  
{  
private:  
    char a;  
    short b;  
    int c;  
public:   
    void printSize()
    {
        std::cout << "sizeof(class CBase4) = " << sizeof(class CBase4) << std::endl;
    }
};

#pragma pack(1)  
class CBase1  
{  
private:  
    char a;  
    short b;  
    int c;  
public:   
    void printSize()
    {
        std::cout << "sizeof(class CBase1) = " << sizeof(class CBase1) << std::endl;
    }
};

int main(void)
{
    CBase4 base4;
    base4.printSize();
     CBase1 base1;
    base1.printSize();
    return 0;
}
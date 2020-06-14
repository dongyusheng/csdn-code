//code_test.cpp
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

#include "IM.BaseDefine.pb.h"
#include "IM.Login.pb.h"

static uint64_t getNowTime()
{
    struct timeval tval;
    uint64_t nowTime;

    gettimeofday(&tval, NULL);

    nowTime = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
    return nowTime;
}

/*
如果proto结构体的变量是基础变量，比如int、string等等，那么set的时候直接调用set_xxx即可。

如果变量是自定义类型(也就是message嵌套)，那么C++的生成代码中，就没有set_xxx函数名，取而代之的是三个函数名：
set_allocated_xxx()
release_xxx()
mutable_xxx()

使用set_allocated_xxx()来设置变量的时候，变量不能是普通栈内存数据，
必须是手动new出来的指针，至于何时delete，就不需要调用者关心了，
protobuf内部会自动delete掉通过set_allocated_设置的内存；

release_xxx()是用来取消之前由set_allocated_xxx设置的数据，
调用release_xxx以后，protobuf内部就不会自动去delete这个数据；

mutable_xxx()返回分配内存后的对象，如果已经分配过则直接返回，如果没有分配则在内部分配，建议使用mutable_xxx
*/
bool ProtobufEncode(std::string &strPb)
{
    IM::Login::Person person;

    person.set_name("dongshao");      // 设置以set_为前缀
    person.set_age(80);

    person.add_languages("C++");    // 数组add
    person.add_languages("Java");

    // 电话号码
    // mutable_ 嵌套对象时使用，并且是单个对象时使用，比如对应的Person里面的Phone  phone = 4;
    // 比如mutable_phone如果phone已经存在则直接返回，如果不存在则new 一个返回
    IM::Login::Phone *phone = person.mutable_phone();
    if(!phone)
    {
        std::cout << "mutable_phone failed." << std::endl;
        return false;
    }
    phone->set_number("18888888888");
    phone->set_phone_type(IM::BaseDefine::PHONE_HOME);

    // 书籍
    // add_则是针对repeated的嵌套对象，每次调用都返回一个新的对象，注意和mutable_的区别。
    // 比如Person里面的repeated Book   books   = 5;
    IM::Login::Book *book = person.add_books();
    book->set_name("Linux kernel development");
    book->set_price(7.7);
    book = person.add_books();
    book->set_name("Linux server development");
    book->set_price(8.0);

    // vip
    person.set_vip(true);
    // 地址
    person.set_address("anhui");

    uint32_t pbSize = person.ByteSize();        // 序列化后的大小
    
    strPb.clear();
    strPb.resize(pbSize);
    uint8_t *szData = (uint8_t *)strPb.c_str();

    if (!person.SerializeToArray(szData, pbSize))   // 拷贝序列化后的数据
    {
        std::cout << "person pb msg SerializeToArray failed." << std::endl;
        return false;
    }
    return true;
}


bool ProtobufDecode(std::string &strPb)
{
    IM::Login::Person person;
    person.ParseFromArray(strPb.c_str(), strPb.size()); // 反序列化

    // printPerson(person); 这个函数在本文件被删除了, 实现可以参阅pb_speed.cpp

    return true;
}

void printHex(uint8_t *data, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++)
    {
        printf("%02x ", data[i]);
    }
    printf("\n\n");
}

void TInt()
{
    std::string strPb;
    uint8_t *szData;
    IM::Login::TInt32 int1;

    uint32_t int1Size = int1.ByteSize();        // 序列化后的大小

    std::cout << "null int1Size = " << int1Size << std::endl;

    int1.set_int1(0x12);
    int1Size = int1.ByteSize();        // 序列化后的大小
    std::cout << "0x12 int1Size = " << int1Size << std::endl;
    strPb.clear();
    strPb.resize(int1Size);
    szData = (uint8_t *)strPb.c_str();
    int1.SerializeToArray(szData, int1Size);   // 拷贝序列化后的数据
    printHex(szData, int1Size);
    
	int1.set_int1(-11);
    int1Size = int1.ByteSize();        // 序列化后的大小
    std::cout << "-11 int1Size = " << int1Size << std::endl;
    strPb.clear();
    strPb.resize(int1Size);
    szData = (uint8_t *)strPb.c_str();
    int1.SerializeToArray(szData, int1Size);   // 拷贝序列化后的数据
    printHex(szData, int1Size);
	

    int1.set_int1(0x7f);
    int1Size = int1.ByteSize();        // 序列化后的大小
    std::cout << "0xff int1Size = " << int1Size << std::endl;
    strPb.clear();
    strPb.resize(int1Size);
    szData = (uint8_t *)strPb.c_str();
    int1.SerializeToArray(szData, int1Size);   // 拷贝序列化后的数据
    printHex(szData, int1Size);
    

    int1.set_int1(0xff);
    int1Size = int1.ByteSize();        // 序列化后的大小
    std::cout << "0xff int1Size = " << int1Size << std::endl;
    strPb.clear();
    strPb.resize(int1Size);
    szData = (uint8_t *)strPb.c_str();
    int1.SerializeToArray(szData, int1Size);   // 拷贝序列化后的数据
    printHex(szData, int1Size);

    int1.set_int1(0x1234);
    int1Size = int1.ByteSize();        // 序列化后的大小
    std::cout << "0x1234 int1Size = " << int1Size << std::endl;
    strPb.clear();
    strPb.resize(int1Size);
    szData = (uint8_t *)strPb.c_str();
    int1.SerializeToArray(szData, int1Size);   // 拷贝序列化后的数据
    printHex(szData, int1Size);

    int1.set_int1(0x123456);
    int1Size = int1.ByteSize();        // 序列化后的大小
    std::cout << "0x123456 int1Size = " << int1Size << std::endl;
    strPb.clear();
    strPb.resize(int1Size);
    szData = (uint8_t *)strPb.c_str();
    int1.SerializeToArray(szData, int1Size);   // 拷贝序列化后的数据
    printHex(szData, int1Size);
}

void TString(void)
{
    std::string strPb;
    uint8_t *szData;
    IM::Login::TString str1;

    uint32_t str1Size = str1.ByteSize();        // 序列化后的大小

    std::cout << "null str1Size = " << str1Size << std::endl;

    str1.set_str1("1");
    str1Size = str1.ByteSize();                 // 序列化后的大小
    std::cout << "1 str1Size = " << str1Size << std::endl;
    strPb.clear();
    strPb.resize(str1Size);
    szData = (uint8_t *)strPb.c_str();
    str1.SerializeToArray(szData, str1Size);   // 拷贝序列化后的数据
    printHex(szData, str1Size);
    
    str1.set_str1("1234");
    str1Size = str1.ByteSize();                 // 序列化后的大小
    std::cout << "1234 str1Size = " << str1Size << std::endl;
    strPb.clear();
    strPb.resize(str1Size);
    szData = (uint8_t *)strPb.c_str();
    str1.SerializeToArray(szData, str1Size);   // 拷贝序列化后的数据
    printHex(szData, str1Size);

    str1.set_str1("老师");
    str1Size = str1.ByteSize();                 // 序列化后的大小
    std::cout << "老师 str1Size = " << str1Size << std::endl;
    strPb.clear();
    strPb.resize(str1Size);
    szData = (uint8_t *)strPb.c_str();
    str1.SerializeToArray(szData, str1Size);   // 拷贝序列化后的数据
    printHex(szData, str1Size);
}

int main(void)
{
    TInt();
    TString();
    return 0;
}

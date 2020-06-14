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

    person.add_languages("C++");    // 数组add 自带类型 protobuff关键字支持的
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

    uint32_t pbSize = person.ByteSize();        // 获取序列化后的大小
    
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

static void printPerson(IM::Login::Person &person)
{
    std::cout << "name:\t" << person.name() << std::endl;
    std::cout << "age:\t" << person.age() << std::endl;
    std::string languages;
    for (int i = 0; i < person.languages_size(); i++)
    {
        if (i != 0)
        {
            languages += ", ";
        }
        languages += person.languages(i);
    }
    std::cout << "languages:\t" << languages << std::endl;

    if (person.has_phone()) // 自定义message的嵌套并且不是设置为repeated则有has_
    {
        // 注意引用
        const IM::Login::Phone &phone = person.phone();
        std::cout << "phone number:\t" << phone.number() << ", type:\t" << phone.phone_type() << std::endl;
    }
    else
    {
        std::cout << "no phone" << std::endl;
    }
    

    for (int i = 0; i < person.books_size(); i++)
    {
        const IM::Login::Book &book = person.books(i);
        std::cout << "book name:\t" << book.name() << ", price:\t" << book.price() << std::endl;
    }

    std::cout << "vip:\t" << person.vip() << std::endl;
    std::cout << "address:\t" << person.address() << std::endl;
}

bool ProtobufDecode(std::string &strPb)
{
    IM::Login::Person person;
    person.ParseFromArray(strPb.c_str(), strPb.size()); // 反序列化

    printPerson(person);

    return true;
}

#define TEST_COUNT 1000000
int main(void)
{
    std::string strPb;
    ProtobufEncode(strPb);  // 序列化后是二进制
    std::cout << "ProtobufDecode, size: " << strPb.size() << std::endl;
    ProtobufDecode(strPb);

#if 1
    uint64_t startTime;
    uint64_t nowTime;

    startTime = getNowTime();
    std::cout << "protobuf encode time testing" << std::endl;
    for (int i = 0; i < TEST_COUNT; i++)
    {
        ProtobufEncode(strPb);
    }
    nowTime = getNowTime();
    std::cout << "protobuf encode " << TEST_COUNT << " time, need time: "
              << nowTime - startTime << "ms" << std::endl;

    startTime = getNowTime();
    std::cout << "protobuf decode time testing" << std::endl;
    for (int i = 0; i < TEST_COUNT; i++)
    {
        ProtobufDecode(strPb);
    }
    nowTime = getNowTime();
    std::cout << "protobuf decode " << TEST_COUNT << " time, need time: "
              << nowTime - startTime << "ms" << std::endl;
#endif
    return 0;
}

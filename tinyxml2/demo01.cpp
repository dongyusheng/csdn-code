/*
读取本地的文件加载到内存
 */
#include <iostream>
#include "tinyxml2.h"

int main(void)
{
    tinyxml2::XMLDocument doc;

    // 本地文件读取
    tinyxml2::XMLError ret = doc.LoadFile("demo01.xml");

    std::cout << doc.ErrorID() << std::endl;
    std::cout << ret << std::endl;

    //  加载到内存
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer); // 打印到内存
    std::cout << printer.CStr() << std::endl;

    std::cout << "size: " << printer.CStrSize() << std::endl;
    std::cout << "size: " << strlen(printer.CStr()) + 1 << std::endl;

    return 0;
}
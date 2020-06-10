#include <iostream>
#include "CharsetConverter.h"

using namespace std;
// g++ -o CharsetConverter main.cpp CharsetConverter.cpp
int main(void)
{
    //string instr = "廖庆富darren";
    string instr = "廖庆富Ȭ¸»𤋮熙darren老师"; // 𤋮超过了0xffff的表示范围，如果用utf8则占用4个字节，GBK不能表示这样的字符.
    // string instr = "i am darren teacher";
    string outstr = "";
    int ret = 0;

    cout << "\norigstr " << instr << endl
         << endl;

    CharsetConverter *converter = new CUtf8ToGbk(); //CUtf16ToUtf8 CUtf8ToGb2312 CUtf8ToGbk

    // 当bErrContinue置为true时, 则iconv转换过程中返回的-1不认为是错误, 只要最终的iconv调用正常则认为结果正常
    // 当bErrContinue置为false, 则iconv转换过程中返回了-1就认为结果出错。
    ret = converter->Convert(instr, outstr, true);
    if (ret != 0)
    {
        cout << "1 Convert failed" << endl;
    }
    cout << "\nutstr " << outstr << ", gbk size:" << outstr.size() << endl;
    delete converter;

    converter = new CGbkToUtf8(); // CUtf16ToUtf8 CGb2312ToUtf8 GbkToUtf8
    instr.clear();
    ret = converter->Convert(outstr, instr, true);
    if (ret != 0)
    {
        cout << "2 Convert failed" << endl;
    }
    cout << "\ninstr " << instr << endl;
    delete converter;
    /*
	
	CharsetConverter *converter = new CUtf8ToUtf16(); //CUtf8ToUtf16 CUtf8ToGb2312 CUtf8ToGbk
   
    ret = converter->Convert(instr, outstr);
    if(ret != 0)
	{
		cout << "1 Convert failed" << endl;
	}
    cout << "\noutstr " << outstr << endl << endl;
    delete converter;

    converter = new CUtf16ToUtf8();	// CUtf16ToUtf8 CGb2312ToUtf8 GbkToUtf8
    instr.clear();
    ret = converter->Convert(outstr, instr);
    if(ret != 0)
	{
		cout << "2 Convert failed" << endl;
	}
    cout << "\ninstr " << instr << endl;
    delete converter;
	*/

    return 0;
}

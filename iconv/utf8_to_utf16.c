//utf8_to_utf16.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>

int main()
{
    unsigned char *encTo = "UTF-16";  //要转换的编码格式
    unsigned char *encFrom = "UTF-8"; //原编码格式

    //新建一个转换分配描述符
    iconv_t cd = iconv_open(encTo, encFrom);
    if(cd == (iconv_t)-1)
    {
        perror("iconv_open");
        return -1;
    }

    //转换的字符串
    unsigned char inbuf[1024] = "CSDN 江南、董少";
    size_t inbufLen = strlen(inbuf);

    //用来保存转换后的字符串
    size_t outbufLen = 1024;
    unsigned char outbuf[outbufLen];
    memset(outbuf, 0, outbufLen);

    //因为iconv()函数会改变inbuf和outbuf指针,因此实现定义两个临时指针指向于这两,然后用这两个指针去操作
    unsigned char *inbufSrc = inbuf;
    unsigned char *outbufSrc = outbuf;

    //转换之前打印一下原字符串和每个字符串的二进制值
    printf("inbuf: %s, len: %ld\n", inbuf, strlen(inbuf));
    printf("utf8:\n");
    for(int i = 0; i < inbufLen; ++i)
    {
        printf("%02x ", inbuf[i]);
    }
    printf("\n\n\n");


    //转换
    size_t ret = iconv(cd, (char**)&inbufSrc, &inbufLen, (char**)&outbufSrc, &outbufLen);
    if(ret == -1)
    {
        perror("iconv");
        iconv_close(cd);
        return -1;
    }


    //打印转换之后的字符串和每个字符串的二进制值(因为outbuf中间会有0，所以不能简单的使用strlen来计算长度)
    printf("outbuf: %s len: %ld\n", outbuf, outbufSrc - outbuf);
    printf("utf16:\n");
    //不能以strlen(outbuf)作为参数2,因为outbuf中间有0，会导致for循环只打印部分的内容
    for(int i = 0; i < (int)(outbufSrc - outbuf); ++i)
    {
        printf("%02x ", outbuf[i]);
    }
    printf("\n");

    //关闭句柄
    iconv_close(cd);
    
    return 0;
}

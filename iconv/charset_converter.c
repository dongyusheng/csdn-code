//charset_converter.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>

typedef struct charset_converter
{
    iconv_t cd; //字符转换描述符
    int error;  //错误码
} charset_converter_t;


/*****************************************************************************
 函 数 名: charset_convert_init
 功能描述  : 创建字符转换描述符,并封装charset_converter_t结构返回
 输入参数  : const char*tocode      ---> 目标编码格式 
           const char *fromcode ---> 源编码格式
 输出参数  : 无
 返 回 值: charset_converter_t * ---> 成功返回charset_converter_t*对象，失败返回NULL
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2020年6月10日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
charset_converter_t *charset_convert_init(const char*tocode, const char *fromcode)
{
    charset_converter_t *converter = (charset_converter_t*)malloc(sizeof(charset_converter_t));
    if(converter == NULL)
    {
        perror("malloc");
        return NULL;
    }
    converter->cd = (iconv_t)-1;
    converter->error = 0;

    converter->cd = iconv_open(tocode, fromcode);
    if(converter->cd == (iconv_t)-1)
    {
        printf("iconv_open failed, errno = %d, %s\n",errno, strerror(errno));
        free(converter);
        return NULL;
    }

    return converter;    
}


/*****************************************************************************
 函 数 名: charset_convert_iconv
 功能描述  : 执行字符集转换
 输入参数  : charset_converter_t *converter --->charset_converter_t*对象
           char **inbuf                   --->源编码缓冲区
           size_t *inbytesleft            --->源编码缓冲区中字符串的大小
           char **outbuf                  --->目标编码缓冲区
           size_t *outbytesleft           --->目标编码缓冲区接收的最大大小
 输出参数  : 无
 返 回 值: int ---> 成功返回0,失败返回-1
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2020年6月10日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
int charset_convert_iconv(charset_converter_t *converter, char **inbuf, size_t *inbytesleft,
                        char **outbuf, size_t *outbytesleft)
{
    if(converter != NULL)
    {
        size_t ret = iconv(converter->cd, inbuf, inbytesleft, outbuf, outbytesleft);
        if(ret == (size_t)-1)
        {
            converter->error = errno;
            printf("iconv failed, errno = %d, %s\n", converter->error, strerror(converter->error));
            switch(converter->error)
            {
            case E2BIG:
                printf("E2BIG\n");
                break;
            case EILSEQ:
                printf("EILSEQ\n");
                break;
            case EINVAL:
                printf("EINVAL\n");
                break;
            default:
                printf("unknown\n");
            }
            return -1;
        }
        return 0;
    }

    printf("converter error\n");
    return -1;
}


/*****************************************************************************
 函 数 名: charset_convert_close
 功能描述  : 释放charset_converter_t结构
 输入参数  : charset_converter_t *converter --->charset_converter_t*对象
 输出参数  : 无
 返 回 值: int ---> 成功返回0,失败返回-1
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2020年6月10日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
int charset_convert_close(charset_converter_t *converter)
{
    if((converter != NULL))
    {
        int ret = iconv_close(converter->cd);
        if(ret == -1)
        {
            perror("iconv_close");
            return -1;
        }

        free(converter);
        return 0;
    }

    printf("converter error\n");
    return -1;
}


int main()
{
    //创建charset_converter_t结构
    const char *encTo = 
"UTF-16";
    const char *encFrom = "UTF-8";
    charset_converter_t *converter = charset_convert_init(encTo, encFrom);
    if(converter == NULL)
        return -1;

    //要转码的字符串
    char inbuf[1024] = "CSDN 江南、董少";
    size_t inbufLen =strlen(inbuf);

    //转码后的保存缓冲区
    size_t outbufLen = 1024;
    char outbuf[outbufLen];
    memset(outbuf, 0, outbufLen);
    

    //转码之前打印一下字符串的相关信息
    printf("inbuf: %s, len: %ld\n", inbuf, strlen(inbuf));
    printf("utf-8:\n");
    for(int i = 0; i < inbufLen; ++i)
    {
        printf("%02x ", inbuf[i]);
    }
    printf("\n\n\n");


    //开始转换
    //因为iconv()函数会改变inbuf和outbuf指针,因此实现定义两个临时指针指向于这两,然后用这两个指针去操作
    char *inbufSrc = inbuf;
    char *outbufSrc = outbuf;

    int ret = charset_convert_iconv(converter, &inbufSrc, &inbufLen, &outbufSrc, &outbufLen);
    if(ret == -1)
        return -1;


    //转码之后打印一下字符串的相关信息
    printf("outbuf: %s len: %ld\n", outbuf, outbufSrc - outbuf);
    printf("utf16:\n");
    for(int i = 0; i < (int)(outbufSrc - outbuf); ++i)
    {
        printf("%02x ", outbuf[i]);
    }
    printf("\n");

    //释放关闭charset_converter_t结构
    ret = charset_convert_close(converter);
    if(ret == -1)
        return -1;

    return 0;
}

//mem_test2.c
#include <stdio.h>
#include <openssl/crypto.h>

int main()
{
    //1.分配内存
    char *str = (char*)OPENSSL_malloc(10);
    if(str == NULL)
    {
        printf("OPENSSL_malloc() error\n");
        return -1;
    }
    printf("OPENSSL_malloc() success\n");

    //2.程序未释放内存, 直接结束程序

    //3.将内存泄漏的信息写入文件中
    //以a模式打开文件,附加的方式打开文件, 只可写. 若文件存在则写入的数据附加在源文件内容后面, 若文件不存在则创建文件
    FILE *fp = fopen("mem_test2.txt", "a");
    int ret = CRYPTO_mem_leaks_fp(fp);
    if(ret == 1)
    {
        printf("No memory leaks\n");
    }
    else if(ret == 0)
    {
        printf("There is a memory leak, please read mem_test2.txt\n");
    }
    else
    {
        printf("OPENSSL_mem_leaks_fp() error\n");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    
    return 0;
}

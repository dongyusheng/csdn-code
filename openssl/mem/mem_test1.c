//mem_test1.c
#include <stdio.h>
#include <string.h>
#include <openssl/crypto.h>

#define STIRNG_LEN 5

int main()
{
    //1.申请一个大小为4的内存
    char *str = (char *)OPENSSL_malloc(4);
    if(str == NULL)
    {
        printf("OPENSSL_malloc error\n");
        return -1;
    }
    printf("OPENSSL_malloc success\n\n");

    //2.将内存全部置为0
    OPENSSL_cleanse(str, 4);
    printf("Set all memory to 0\n\n");
    
    //3.将字符串拷贝到str中, 并打印
    memcpy(str, "abc", 4);
    printf("str: %s\n\n", str);

    //4.将原先的内存全部置为0, 然后再重新申请一块内存
    str = OPENSSL_clear_realloc(str, 4, 10);
    if(str == NULL)
    {
        printf("OPENSSL_clear_realloc error\n");
        return -1;
    }
    printf("OPENSSL_clear_realloc success\n\n");

    //5.将内存全部置为0
    OPENSSL_cleanse(str, 4);
    printf("Set all memory to 0\n\n");
    
    //6.将字符串拷贝到str中, 并打印
    memcpy(str, "Github", 7);
    printf("str: %s\n\n", str);

    //7.释放内存
    OPENSSL_free(str);
    
    return 0;
}

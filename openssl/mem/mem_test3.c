//mem_test3.c
#include <stdio.h>
#include <openssl/crypto.h>

int main()
{
    //1.分配内存
    char *str1 = (char*)OPENSSL_malloc(10);
    if(str1 == NULL)
    {
        printf("OPENSSL_malloc() error\n");
        return -1;
    }
    printf("OPENSSL_malloc() success\n");

    //2.分配内存
    char *str2 = (char*)OPENSSL_malloc(10);
    if(str2 == NULL)
    {
        printf("OPENSSL_malloc() error\n");
        return -1;
    }
    printf("OPENSSL_malloc() success\n");

    //3.为str1重分配内存
    str1 = OPENSSL_realloc(str1, 20);
    if(str1 == NULL)
    {
        printf("OPENSSL_realloc() error\n");
        return -1;
    }
    printf("OPENSSL_realloc() success\n");

    //4.释放两个内存
    OPENSSL_free(str1);
    OPENSSL_free(str2);

    //5.打印内存的使用信息
    int m, r, f;
    CRYPTO_get_alloc_counts(&m, &r, &f);
    printf("information:\n\t, malloc count: %d\n\t, realloc count: %d\n\t, free count: %d\n",m, r, f);
    
    return 0;
}

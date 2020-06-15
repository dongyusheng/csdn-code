//hash_test2.c
https://github.com/dongyusheng/csdn-code/tree/master/openssl/hash
#include <stdio.h>
#include <string.h>
#include <openssl/lhash.h>

unsigned long hashff(void *hf)
{
    printf("%s\n",hf);
    return 100;
}

int hashfCmp(int  *a,int  *b)
{
    return *a > *b;
    
}

void printArg(int *a,char *b)
{
    printf("doall_arg: %d  %s\n",*a,b);
}

void printValue(int *value)
{
    printf("doall: %d\n",*value);
}


int main(int argc, const char * argv[]) {
    
    
    OPENSSL_LHASH *lh = OPENSSL_LH_new(NULL, NULL);
    
    int item = 1;
    OPENSSL_LH_insert(lh, &item);
    
    
    int item2 = 10;
    OPENSSL_LH_insert(lh, &item2);
    
    int item3 = 5;
    OPENSSL_LH_insert(lh, &item3);
    
    
    //因为表中已经存在数据5，如果再插入，将会替换之前的数据5
    int item4 = 5;
    int *ret=0;
    ret = OPENSSL_LH_insert(lh, &item4);
    if (*ret==item4) {
        printf("insert replace PASS\n");
    }
    
    
    int *fd = 0;
    fd = OPENSSL_LH_retrieve(lh,&item2);
    if (*fd == item2) {
        printf("find value PASS\n");
    }
    
    OPENSSL_LH_doall(lh, printValue);
    OPENSSL_LH_doall_arg(lh, printArg, "arg");
    
    
    int *delRet = 0;
    delRet = OPENSSL_LH_delete(lh, &item4);
    if (*delRet==item4) {
        printf("delete value PASS\n");
    }
    
    
    int numLen = OPENSSL_LH_num_items(lh);
    printf("len=%d\n");
    
    
    OPENSSL_LH_stats(lh, stdout);
    
    
    OPENSSL_LH_free(lh);
    
    
    
    return 0;
}

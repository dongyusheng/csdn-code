#include <stdio.h>
#include <string.h>
#include <openssl/lhash.h>

#define NAME_LENGTH 32

typedef struct _Person
{
    char name[NAME_LENGTH];
    double hight;
} Person;


int personCompare(const void *arg1, const void *arg2)
{
    char *name1 = ((Person*)arg1)->name;
    char *name2 = ((Person*)arg2)->name;

    return strcmp(name1, name2);
}

void printNode(void *arg)
{
    Person *person = (Person*)arg;
    printf("name :%s, height: %f\n", person->name, person->hight);
}

int main()
{
    //1.创建哈希表
    OPENSSL_LHASH *_hash = OPENSSL_LH_new(NULL, personCompare);
    if(_hash == NULL)
    {
        printf("OPENSSL_LH_new error\n");
        return -1;
    }

    //2.向哈希表中插入三条数据
    Person person1 = {"Tom", 170.5};
    Person person2 = {"Joke", 180.5};
    Person person3 = {"Alan", 175.5};
    OPENSSL_LH_insert(_hash, &person1);
    OPENSSL_LH_insert(_hash, &person2);
    OPENSSL_LH_insert(_hash, &person3);

    //3.遍历所有的数据
    OPENSSL_LH_doall(_hash, printNode);
    printf("\n\n");

    //4.删除一条数据之后再遍历
    if(OPENSSL_LH_delete(_hash, (const void*)"Tom") == NULL)
    {
        printf("OPENSSL_LH_delete error\n");
        return -1;
    }
    OPENSSL_LH_doall(_hash, printNode);
    printf("\n\n");

    //5.查找一条数据
    void *findData = OPENSSL_LH_retrieve(_hash, "Alan");
    if(findData == NULL)
    {
        printf("OPENSSL_LH_retrieve error\n");
        return -1;
    }
    printf("find success:\n\t");
    printNode(findData);


    //6.销毁哈希表
    OPENSSL_LH_free(_hash);

    return 0;
}


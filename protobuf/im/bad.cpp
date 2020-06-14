/*
 * @Author: your name
 * @Date: 2020-05-14 18:01:26
 * @LastEditTime: 2020-05-14 20:24:11
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \im\bad.cpp
 */ 
#include <iostream>
#include <stdio.h>

struct Person1
{
    char name[15];      // 姓名  16 + 4 + 24
    int age;  // 年龄
    char phone[21];    // 电话
};

struct Person2
{
    char name[15];      // 姓名 15  + 4 + 21 = 40
    int age;  // 年龄
    char phone[21];
    /* data */
}__attribute__((packed));;

/*
sizeof(Persion1) = 44
sizeof(Persion2) = 40
*/
int main() 
{
    printf("sizeof(Person1) = %lu\n", sizeof(struct Person1));
    printf("sizeof(Person2) = %lu\n", sizeof(struct Person2));
    return  0;
}
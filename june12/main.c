#include<stdio.h>
#include<stdlib.h>
int add(int a,int b)
{
    return a+b;
}
int sub(int a,int b)
{
    return a-b;
}
int main()
{
    int (*fp[2])(int,int)={add,sub};
    printf("%d",fp[0](1,2));
    printf("%d",fp[1](1,2));
}
#include<stdio.h>
#include<stdlib.h>
enum handler{ADD,SUB,MUL};
int add(int a,int b)
{
    return a+b;
}
int sub(int a,int b)
{
    return a-b;
}
int mul(int a,int b)
{
    return a*b;
}
int main(){
    int(*fp[3])(int,int)={add,sub,mul};
    int a,b,ch;
    printf("enter a nd b:");
    scanf("%d %d",&a,&b);
   printf("enter a number between 0 or 2 to perform operation:");
   scanf("%d",&ch);
    switch(ch)
    {
        case 0:
                    printf("%d",fp[0](a,b));
                    break;
        case 1:
                    printf("%d",fp[1](a,b));
                    break;
        case 2:
                    printf("%d",fp[2](a,b));
                    break;
        default:printf("invalid");
    }
}
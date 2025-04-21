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
int mul(int a,int b)
{
    return a*b;
}
int main(){
    int(*fp[3])(int,int)={add,sub,mul};
    int a,b;
    char ch;
     printf("enter any symbol between '+' or '-' or '*'\n");
    scanf("%c",&ch);
    printf("enter a nd b:");
    scanf("%d %d",&a,&b);
   
    switch(ch)
    {
        case '+':
                    printf("%d",fp[0](a,b));
                    break;
        case '-':
                    printf("%d",fp[1](a,b));
                    break;
        case '*':
                    printf("%d",fp[2](a,b));
                    break;
        default:printf("invalid");
    }
}
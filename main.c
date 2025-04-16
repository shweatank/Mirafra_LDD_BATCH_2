// Online C compiler to run C program online
#include <stdio.h>
typedef int (*opratorptr)(int ,int);
char operators[]={'+','-','*','/'};
int operatorcheck(char c)
{
    for(int i=0;i<sizeof(operators);i++)
    {
        if(c==operators[i])
        {
            return i;
        }
    }
}
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
int div(int a,int b)
{
    return a/b;
}
opratorptr table[]={add,sub,mul,div};
int main() {
    int a,b;
    char c;
    printf("enter in <int> <op> <int> format\n");
   while(1)
   {
       scanf("%d%c%d",&a,&c,&b);
       int val=operatorcheck(c);
       printf("result is : %d\n",table[val](a,b));
   }
}

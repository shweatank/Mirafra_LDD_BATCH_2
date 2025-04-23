#include <stdio.h>
int add(int,int);
int sub(int,int);
int display(void(* ptr)(int,int));
int main()
{
    printf("Hello World\n");
    display(add);
    display(sub);
    return 0;
}

int add(int a,int b)
{
    printf("sun=%d\n",a+b);
}
int sub(int a,int b)
{
    printf("diff=%d",a-b);
}

int display(void(* ptr)(int,int))
{
    (* ptr)(3,2);
}

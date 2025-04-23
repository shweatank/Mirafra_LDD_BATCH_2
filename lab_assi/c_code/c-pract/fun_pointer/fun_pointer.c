#include <stdio.h>
int add(int,int);
int sub(int,int);
int main()
{
    printf("Hello World\n");
    int (*ptr[2])(int,int)={&add,&sub};
    int z=(* ptr[0])(2,3);
    printf("%d",z);
    return 0;
}

int add(int a,int b)
{
    return (a+b);
}
int sub(int a,int b)
{
    return a-b;
}

//WAP TO GENERATE NEGATIVE FIBONACCI NUMBERS IN RECURSIVE METHOD
#include <stdio.h>
int recurs_fib(int num, int a, int b)
{
    if(a < num)
    {
        printf("%d ", a);
        recurs_fib(num, b,a-b);
    }
}
int main()
{
    int num ;
    scanf("%d", &num);
    
    recurs_fib(num, 0,1);
}

#include <stdio.h>
float add(int a, int b)
{
    return a+b;
}
float sub(int a, int b)
{
    return a-b;
}
float mul(int a, int b)
{
    return a*b;
}
float Div(int a, int b)
{
    return a/b;
}
int main()
{
    float(*ptr[])(int, int) = {add, sub, mul, Div};
    int a ,b;
    printf("Enter a and b \n");
    scanf("%d %d", &a, &b);
    
    int op;
    printf("Enter the op value \n");
    scanf("%d", &op);
    
    if(op >= 1 && op <= 4)
    {
        printf("%.2f", (*ptr[op - 1])(a,b));
        return 0;
    }
    else{
        printf("INvalid OP value \n");
        return -1;
    }
}

#include <stdio.h>


int add(int a, int b) 
{
    return a + b;
}
int sub(int a, int b) 
{
    return a - b;
}
int mul(int a, int b) 
{
    return a * b;
}
int divide(int a, int b) 
{
    if (b != 0)
        return a / b;
    else {
        printf("Division by zero error!\n");
        return 0;
    }
}

int main() {
    int a = 20, b = 10;

    
    int (*op[4])(int, int) = {add, sub, mul, divide};

    
    char *ops[] = {"Addition", "Subtraction", "Multiplication", "Division"};

  
    for (int i = 0; i < 4; ++i) 
    {
        printf("%s of %d and %d = %d\n", ops[i], a, b, op[i](a, b));
    }

    return 0;
}


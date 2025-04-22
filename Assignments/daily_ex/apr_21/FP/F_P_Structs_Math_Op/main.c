#include <stdio.h>

struct MathOps {
    int (*add)(int, int);
    int (*subtract)(int, int);
    int (*multiply)(int, int);
    int (*divide)(int, int);
};

int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b)   { return b != 0 ? a / b : 0; }

int main() {
    struct MathOps ops = {add, subtract, multiply, divide};
    int a = 20, b = 5;

    printf("add: %d\n", ops.add(a, b));
    printf("subtract: %d\n", ops.subtract(a, b));
    printf("multiply: %d\n", ops.multiply(a, b));
    printf("divide: %d\n", ops.divide(a, b));

    return 0;
}


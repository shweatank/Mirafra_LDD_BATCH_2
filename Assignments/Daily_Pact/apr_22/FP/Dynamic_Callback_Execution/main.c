#include <stdio.h>

int add(int a, int b)    { return a + b; }
int mul(int a, int b)    { return a * b; }

int execute(int (*op)(int, int), int a, int b) {
    return op(a, b);
}

int main() {
    printf("10 + 20 = %d\n", execute(add, 10, 20));
    printf("10 * 20 = %d\n", execute(mul, 10, 20));
    return 0;
}


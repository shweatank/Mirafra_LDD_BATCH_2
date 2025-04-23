#include <stdio.h>

int square(int x) { return x * x; }
int cube(int x)   { return x * x * x; }

int main() {
    int (*funcs[])(int) = {square, cube};
    int nums[] = {1, 2, 3, 4};

    for (int i = 0; i < 4; ++i) {
        printf("num: %d, square: %d, cube: %d\n", nums[i], funcs[0](nums[i]), funcs[1](nums[i]));
    }

    return 0;
}


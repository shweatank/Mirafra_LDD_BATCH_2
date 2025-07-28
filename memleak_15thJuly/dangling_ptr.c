#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(10 * sizeof(int));
    free(ptr);
    ptr[0] = 42;  // use after free
    return 0;
}


#include <stdlib.h>

int main() {
    int *arr = malloc(10 * sizeof(int));  // Allocated but not freed (memory leak)
    arr[10] = 5;  // Invalid write (out-of-bounds)
    return 0;
}


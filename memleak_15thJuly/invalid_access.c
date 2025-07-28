#include <stdlib.h>

int main() {
    int *arr = malloc(5 * sizeof(int));
    arr[5] = 100;  // Out-of-bounds write
    return 0;
}


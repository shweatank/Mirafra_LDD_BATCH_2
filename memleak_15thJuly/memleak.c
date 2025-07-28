#include <stdlib.h>

void leak() {
    int *ptr = malloc(10);  // allocated but never freed!
    ptr[0] = 10;
    free(ptr);
}

int main() {
    for (int i = 0; i < 10; i++) {
        leak();
    }
    return 0;
}


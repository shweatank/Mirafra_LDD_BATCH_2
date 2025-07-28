
#include <stdlib.h>

int main() {
    int *ptr = malloc(100);
    free(ptr);
    free(ptr);  // Double free
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_leak() {
    char *ptr1 = malloc(50);          // Allocate 50 bytes
    strcpy(ptr1, "This is a memory leak example.");

    char *ptr2 = malloc(100);         // Allocate 100 bytes
    strcpy(ptr2, "Another leaked block.");

    // Free ptr2 but NOT ptr1 => intentional leak
    free(ptr2);
    free(ptr1);
}

int main() {
    printf("Running memory leak example...\n");
    create_leak();
    printf("Done.\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_leak()
{
    // Allocate memory but never free it
    char *ptr = malloc(100);
    if (ptr == NULL) {
        perror("malloc failed");
        return;
    }
    strcpy(ptr, "This memory will be leaked!");
    printf("Leaked string: %s\n", ptr);

    // Intentionally no free(ptr);
}

int main()
{
    for (int i = 0; i < 5; ++i) {
        create_leak();
    }

    printf("Program finished.\n");
    return 0;
}


#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./my_mv <source> <destination>\n");
        return 1;
    }

    if (rename(argv[1], argv[2]) != 0) {
        perror("rename failed");
        return 1;
    }

    printf("Moved '%s' to '%s'\n", argv[1], argv[2]);
    return 0;
}


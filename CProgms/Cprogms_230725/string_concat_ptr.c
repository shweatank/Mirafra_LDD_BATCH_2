#include <stdio.h>

void my_strcat(char *dest, const char *src) {
    // Move dest pointer to the end of existing string
    while (*dest != '\0') {
        dest++;
    }

    // Copy characters from src to dest
    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0'; // Null-terminate the concatenated string
}

int main() {
    char str1[100] = "Hello ";
    char str2[] = "World!";
    printf(" s1 - %s  and s2 -  %s\n", str1, str2);
    my_strcat(str1, str2);

    printf("\nConcatenated String: %s\n", str1);

    return 0;
}


#include <stdio.h>

// Custom implementation of strlen()
int my_strlen(const char *str) {
    int length = 0;
    while (*str++)
        length++;
    return length;
}

// Custom implementation of strcpy()
char* my_strcpy(char *dest, const char *src) {
    char *ptr = dest;
    while (*ptr != '\0'){
      *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}

// Custom implementation of strcmp()
int my_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int main() {
    char src[] = "ganga";
    char dest[20];

    printf("Length of '%s' = %d\n", src, my_strlen(src));

    my_strcpy(dest, src);
    printf("Copied string: %s\n", dest);

    printf("Compare 'ganga' and 'ganga': %d\n", my_strcmp("ganga", "ganga"));  // 0
    printf("Compare 'riyan' and 'riya': %d\n", my_strcmp("riyan", "riya"));  // < 0
    printf("Compare 'zebra' and 'apple': %d\n", my_strcmp("zebra", "apple"));  // > 0

    return 0;
}


#include <stdio.h>

int main() {
    char str[200];
    int i = 0, start = 0, len = 0;
    int maxStart = 0, maxLen = 0;

    printf("Enter a string: ");
    fgets(str, sizeof(str), stdin);

    while (str[i] != '\0') {
        // if not space and not newline
        if (str[i] != ' ' && str[i] != '\n') {
            if (len == 0)
                start = i; // mark the start of word
            len++;
        } else {
            if (len > maxLen) {
                maxLen = len;
                maxStart = start;
            }
            len = 0; // reset for next word
        }
        i++;
    }

    // Check last word (if no space at end)
    if (len > maxLen) {
        maxLen = len;
        maxStart = start;
    }

    printf("Longest word: ");
    for (int j = 0; j < maxLen; j++) {
        printf("%c", str[maxStart + j]);
    }
    printf("\nLength: %d\n", maxLen);

    return 0;
}


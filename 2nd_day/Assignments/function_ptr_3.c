//Word Analyzer Tool Using Function Pointers

#include <stdio.h>
#include <string.h>
#include <ctype.h>

void count_vowels(char* str) {
    int count = 0;
    for (int i = 0; str[i]; i++) {
        char c = tolower(str[i]);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') count++;
    }
    printf("Vowel count: %d\n", count);
}

void count_words(char* str) {
    int count = 0;
    int inWord = 0;
    for (int i = 0; str[i]; i++) {
        if (isspace(str[i])) {
            inWord = 0;
        } else if (!inWord) {
            inWord = 1;
            count++;
        }
    }
    printf("Word count: %d\n", count);
}

void count_chars(char* str) {
    printf("Character count (excluding spaces): %ld\n", strlen(str) - strspn(str, " "));
}

int main() {
    void (*analyzers[3])(char*) = {count_vowels, count_words, count_chars};
    char input[256];
    int option;

    printf("Enter a string to analyze: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    printf("Choose analysis:\n");
    printf("1. Count Vowels\n2. Count Words\n3. Count Characters\n");
    scanf("%d", &option);

    if (option >= 1 && option <= 3) {
        analyzers[option - 1](input);
    } else {
        printf("Invalid option.\n");
    }

    return 0;
}


#include <stdio.h>
#include <string.h>
#include <ctype.h>

void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++)
        str[i] = toupper(str[i]);
}

void to_lowercase(char* str) {
    for (int i = 0; str[i]; i++)
        str[i] = tolower(str[i]);
}

void reverse_string(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main() {
    void (*functions[3])(char*) = {to_uppercase, to_lowercase, reverse_string};
    char input[100];
    int choice;

    printf("Enter a string: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    printf("Choose an operation:\n");
    printf("1. To Uppercase\n2. To Lowercase\n3. Reverse String\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice >= 1 && choice <= 3) {
        functions[choice - 1](input);
        printf("Result: %s\n", input);
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}


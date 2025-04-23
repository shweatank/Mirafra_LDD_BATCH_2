#include <stdio.h>

struct Book {
    char title[100];
    char author[50];
    float price;
};

void printBook(struct Book b) {
    printf("Title: %s\n", b.title);
    printf("Author: %s\n", b.author);
    printf("Price: %.2f\n", b.price);
}

int main() {
    struct Book book = {"LDD practice codes", "Akash R", 29.99};
    printBook(book);
    return 0;
}


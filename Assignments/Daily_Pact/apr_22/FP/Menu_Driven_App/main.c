#include <stdio.h>

void add()    { printf("adding item...\n"); }
void delete() { printf("deleting item...\n"); }
void view()   { printf("viewing items...\n"); }

int main() {
    void (*menu[])() = {add, delete, view};
    int choice;

    while (1) {
        printf("\n0. Add\n1. delete\n2. view\n3. Exit\nchoice: ");
        scanf("%d", &choice);

        if (choice == 3) break;
        if (choice >= 0 && choice < 3)
            menu[choice]();
        else
            printf("invalid choice.\n");
    }

    return 0;
}


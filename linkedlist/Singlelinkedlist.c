#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *link;
};

void Count_Nodes(struct node *head, int *count) {
    if (head == NULL) {
        *count = 0;
        return;
    }

    struct node *ptr = head;
    *count = 0;
    while (ptr != NULL) {
        (*count)++;
        ptr = ptr->link;
    }
}

void Print_data(struct node *head) {
    if (head == NULL) {
        printf("List is empty.\n");
        return;
    }

    struct node *ptr = head;
    while (ptr != NULL) {
        printf("%d\n", ptr->data);
        ptr = ptr->link;
    }
}

int main() {
    int count = 0;
    int data;

    struct node *head = NULL;
    head = (struct node*)malloc(sizeof(struct node)); // Correct memory allocation
    head->data = 45;
    head->link = NULL;

    struct node *current = malloc(sizeof(struct node)); // Correct memory allocation
    current->data = 32;
    current->link = NULL;
    head->link = current;

    current = malloc(sizeof(struct node)); // Correct memory allocation
    current->data = 100;
    current->link = NULL;
    head->link->link = current;

    current = malloc(sizeof(struct node *));
    current -> data = 74;
    current -> link = NULL;
    head -> link -> link-> link = current;

    // Count the number of nodes
    Count_Nodes(head, &count);
    printf("Number of nodes: %d\n", count);

    // Print the data of each node
    printf("Data in the list:\n");
    Print_data(head);
    printf("Single Linked List\n");

    return 0;
}


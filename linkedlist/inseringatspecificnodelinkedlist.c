#include <stdio.h>                                                                                                                                                       
#include <stdlib.h>

struct node { 
    int data;
    struct node *link;
};

void add_at_specific_pos(struct node *head, int data, int pos) {
    struct node *ptr = head;
    struct node *ptr2 = malloc(sizeof(struct node));

    ptr2->data = data;
    ptr2->link = NULL;

    int temp = pos - 1;


    while (temp != 1) 
    {
        ptr = ptr->link;
        temp--;
    }

    ptr2->link = ptr->link;
    ptr->link = ptr2;
}

int main() {
    struct node *head = malloc(sizeof(struct node));
    head->data = 34;
    head->link = NULL;

    struct node *current = malloc(sizeof(struct node));
    current->data = 56;
    current->link = NULL;
    head->link = current;  

    current = malloc(sizeof(struct node)); 
    current->data = 78;
    current->link = NULL;

    head->link->link = current;

    struct node *ptr = head;

    add_at_specific_pos(head, 89, 3);  // Insert 89 at position 2

    while (ptr != NULL) {
        printf("%d\n", ptr->data);
        ptr = ptr->link;
    }

    return 0;
}


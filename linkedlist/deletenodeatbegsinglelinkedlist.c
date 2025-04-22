#include <stdio.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *link;
};

struct node *del_first(struct node *head) {
    if (head == NULL) {
        printf("List is Empty\n");
        return NULL; 
    } else {
        struct node *temp = head;
        head = head->link;
        free(temp);
        return head;
    }
}

int main() {
    
    struct node *head = malloc(sizeof(struct node));
    head->data = 21;
    head->link = NULL;

    
    struct node *current = malloc(sizeof(struct node));
    current->data = 34;
    current->link = NULL;
    head->link = current;

    
    current = malloc(sizeof(struct node));
    current->data = 56;
    current->link = NULL;
    head->link->link = current;

    
    head = del_first(head);

    
    struct node *ptr = head;
    while (ptr != NULL) {
        printf("%d\n", ptr->data);
        ptr = ptr->link;
    }

    return 0;
}


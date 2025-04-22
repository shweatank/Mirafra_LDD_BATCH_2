#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* next;
};

struct Node* reverseList(struct Node* head) {
    struct Node *prev = NULL, *current = head, *next;
    while (current) {
        next = current->next; 
        current->next = prev; 
        prev = current;       
        current = next;  
    }
    return prev; 
}

struct Node* newNode(int data) {
    struct Node* node = malloc(sizeof(struct Node));
    node->data = data;
    node->next = NULL;
    return node;
}

void printList(struct Node* head) {
    while (head) {
        printf("%d -> ", head->data);
        head = head->next;
    }
    printf("NULL\n");
}

int main() {
struct Node* head = newNode(1);
head->next = newNode(2);
head->next->next = newNode(3);
head->next->next->next = newNode(4);
printf("before ");
printList(head);
head = reverseList(head);
printf("after: ");
printList(head);
return 0;
}

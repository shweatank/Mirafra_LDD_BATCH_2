#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

void apply(Node *head, void (*func)(int *)) {
    while (head) {
        func(&head->data);
        head = head->next;
    }
}

void double_val(int *x) {
    *x *= 2;
}

void print_list(Node *head) {
    while (head) {
        printf("%d ", head->data);
        head = head->next;
    }
    puts("");
}

Node *push(Node *head, int val) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = val;
    new_node->next = head;
    return new_node;
}

int main() {
    Node *head = NULL;
    head = push(head, 10);
    head = push(head, 20);
    head = push(head, 30);

    apply(head, double_val);
    print_list(head);
    return 0;
}


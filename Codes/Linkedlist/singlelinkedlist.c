#include <stdio.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *link;
};

// Insert at beginning
struct node* insertAtBeginning(struct node *head, int value) {
    struct node *newnode = (struct node *)malloc(sizeof(struct node));
    newnode->data = value;
    newnode->link = head;
    return newnode;
}

// Insert at end
struct node* insertAtEnd(struct node *head, int value) {
    struct node *newnode = (struct node *)malloc(sizeof(struct node));
    newnode->data = value;
    newnode->link = NULL;

    if (head == NULL)
        return newnode;

    struct node *temp = head;
    while (temp->link != NULL)
        temp = temp->link;

    temp->link = newnode;
    return head;
}

// Insert at specific position
struct node* insertAtPosition(struct node *head, int pos, int value) {
    if (pos == 1)
        return insertAtBeginning(head, value);

    struct node *newnode = (struct node *)malloc(sizeof(struct node));
    newnode->data = value;

    struct node *temp = head;
    for (int i = 1; temp != NULL && i < pos - 1; i++)
        temp = temp->link;

    if (temp == NULL) {
        printf("Position out of bounds!\n");
        free(newnode);
        return head;
    }

    newnode->link = temp->link;
    temp->link = newnode;
    return head;
}

// Delete at beginning
struct node* deleteAtBeginning(struct node *head) {
    if (head == NULL) {
        printf("List is empty!\n");
        return NULL;
    }

    struct node *temp = head;
    head = head->link;
    free(temp);
    return head;
}

// Delete at end
struct node* deleteAtEnd(struct node *head) {
    if (head == NULL) {
        printf("List is empty!\n");
        return NULL;
    }

    if (head->link == NULL) {
        free(head);
        return NULL;
    }

    struct node *temp = head;
    while (temp->link->link != NULL)
        temp = temp->link;

    free(temp->link);
    temp->link = NULL;
    return head;
}

// Delete at specific position
struct node* deleteAtPosition(struct node *head, int pos) {
    if (head == NULL) {
        printf("List is empty!\n");
        return NULL;
    }

    if (pos == 1)
        return deleteAtBeginning(head);

    struct node *temp = head;
    for (int i = 1; temp->link != NULL && i < pos - 1; i++)
        temp = temp->link;

    if (temp->link == NULL) {
        printf("Position out of bounds!\n");
        return head;
    }

    struct node *del = temp->link;
    temp->link = del->link;
    free(del);
    return head;
}

// Reverse the linked list
struct node* reverseList(struct node *head) {
    struct node *prev = NULL, *curr = head, *next = NULL;
    while (curr != NULL) {
        next = curr->link;
        curr->link = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

// Print the linked list
void printList(struct node *head) {
    struct node *temp = head;
    while (temp != NULL) {
        printf("%d->", temp->data);
        temp = temp->link;
    }
    printf("NULL\n");
}

// Main function
int main() {
    struct node *head = NULL, *newnode, *temp;
    int n, i, value, pos;

    // Linked list creation
    printf("Enter number of nodes: ");
    scanf("%d", &n);

    for (i = 1; i <= n; i++) {
        newnode = (struct node *)malloc(sizeof(struct node));
        printf("Enter data for node %d: ", i);
        scanf("%d", &newnode->data);
        newnode->link = NULL;

        if (head == NULL) {
            head = newnode;
            temp = newnode;
        } else {
            temp->link = newnode;
            temp = newnode;
        }
    }

    printList(head);

    // Insert at beginning
    printf("Enter value to insert at beginning: ");
    scanf("%d", &value);
    head = insertAtBeginning(head, value);
    printList(head);

    // Insert at end
    printf("Enter value to insert at end: ");
    scanf("%d", &value);
    head = insertAtEnd(head, value);
    printList(head);

    // Insert at position
    printf("Enter position and value to insert: ");
    scanf("%d %d", &pos, &value);
    head = insertAtPosition(head, pos, value);
    printList(head);

    // Delete at beginning
    printf("Deleting at beginning...\n");
    head = deleteAtBeginning(head);
    printList(head);

    // Delete at end
    printf("Deleting at end...\n");
    head = deleteAtEnd(head);
    printList(head);

    // Delete at position
    printf("Enter position to delete: ");
    scanf("%d", &pos);
    head = deleteAtPosition(head, pos);
    printList(head);

    // Reverse the list
    printf("Reversing the list...\n");
    head = reverseList(head);
    printList(head);

    return 0;
}


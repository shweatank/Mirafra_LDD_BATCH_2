#include "main.h"

int insert_at_last(dlist **head, dlist **tail, int ndata) {
    dlist *nnode = (dlist *)malloc(sizeof(dlist));
    if (nnode == NULL) {
        return failure;
    }
    nnode->data = ndata;
    nnode->next = NULL;
    nnode->prev = NULL;

    if (*head == NULL) {  // If list is empty, initialize it
        *head = *tail = nnode;
        (*head)->prev = NULL;
        (*tail)->next = NULL;  // Ensuring tail's next is NULL
        return success;
    } else {
        nnode->prev = *tail;
        (*tail)->next = nnode;  // Fix: Set the next pointer of the current tail
        *tail = nnode;
        return success;
    }
}
#include "main.h"

int print_list(dlist *head) {
    if (head == NULL) {
        return failure;
    }
    printf("(HEAD) > ");
    dlist *temp = head;
    while (temp) {
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    printf(" (TAIL)\n");
    return success;
}
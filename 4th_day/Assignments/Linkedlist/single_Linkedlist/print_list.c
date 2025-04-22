#include "main.h"

int printlist(slist *head)
{
    if(head == NULL)
    {
        return failure;
    }
    printf("Head ->");
    slist *temp = head;
    while(temp != NULL)
    {
        printf("%d ->", temp->data);
        temp = temp->link;
    }
    printf("NULL");
    return success;
}
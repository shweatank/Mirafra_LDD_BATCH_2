#include "main.h"

int Delete_at_last(slist **head)
{
    if(*head == NULL)
    {
        return failure;
    }
    if((*head)->link ==NULL)
    {
        free(*head);
        *head = NULL;
        return success;
    }
    slist *temp = *head;
    slist *prev=  NULL;
    while(temp->link != NULL)
    {
        prev = temp;
        temp = temp->link;
    }
    prev->link = NULL;
    free(temp);
    return success;
}
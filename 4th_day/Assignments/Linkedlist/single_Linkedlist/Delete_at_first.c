#include "main.h"

int Delete_at_first(slist **head)
{
    if(*head == NULL)
    {
        return failure;
    }
    
    slist *temp = *head;
    *head = temp->link;
    free(temp);
    return success;
}
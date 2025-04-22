#include "main.h"

int Delete_list(slist *head)
{
    if(head == NULL)
    {
        return failure;
    }
    slist *temp = head;
    while (temp != NULL)
    {
        free(temp);
        temp = temp->link;
        return success;
    }
    return failure;
    
}
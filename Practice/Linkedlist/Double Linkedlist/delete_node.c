#include "main.h"

int delete_node(dlist **head, dlist **tail, int ndata)
{
    if(*head == NULL)
    {
        return failure;
    }
    
    dlist *temp = *head;
    
    //if node is first node
    if(temp->data == ndata)
    {
        *head = temp->next;
        if(*head != NULL)
        {
            (*head)->prev = NULL;
        }
        else
        {
            *tail = NULL;
        }
        free(temp);
        return success;
    }
    
    while(temp != NULL)
    {
        if(temp->data == ndata)
        {
            //last node
            if(temp == *tail)
            {
                *tail = temp->prev;
                (*tail)->next = NULL;
                free(temp);
                return success;
                
            }
            else
            {
                //middle node
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                free(temp);
                return success;
                
            }
        }
        else
        {
            temp = temp->next;
        }
    }
}
#include "main.h"

int insert_at_last(slist **head, int ndata)
{
    slist *nnode = (slist *)malloc(sizeof(slist));
	if(nnode == NULL)
	{
		return failure;
	}
	nnode->data = ndata;
	nnode->link = NULL;
	
	if(*head == NULL)
	{
		*head = nnode;
		return success;
	}
    slist *temp = *head;
    while(temp->link != NULL)
    {
        temp = temp->link;
    }
    temp->link = nnode;
    return success;
}
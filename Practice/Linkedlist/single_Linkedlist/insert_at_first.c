#include "main.h"

int insert_at_first(slist **head, int ndata)
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
	nnode->link = *head;
	*head = nnode;
	return success;
}

#include "main.h"

int reverse_double_linkedlist(dlist **head)
{
	if(*head == NULL)
	{
	 return failure;
	}
	
	dlist *current = *head;
	dlist *temp = NULL;

	while(current)
	{
	
	temp = current->prev;
	current->prev = current->next;
	current->next = temp;

	current = current->prev;

	}
		
	if(temp)
	{
	*head = temp->prev;
	}
}

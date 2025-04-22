#include "main.h"

int delete_last(dlist **head, dlist **tail)
{
  if(*head == NULL)
  {
      return failure;
  }
  if((*head)->next == NULL)
  {
      free(*head);
      *head = *tail = NULL;
  }
  
  dlist *temp = *head;
  while(temp->next != NULL)
  {
      temp = temp->next;
  }
  
  *tail = temp->prev;
  (*tail)->next = NULL;
  free(temp);
  return success;
}
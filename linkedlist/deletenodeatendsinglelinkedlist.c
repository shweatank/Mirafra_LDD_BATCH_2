#include <stdio.h>
#include <stdlib.h>

struct node
{
  int data;
  struct node *link;

};

struct node *del_end(struct node *head)
{
  if(head == NULL)
  {
    printf("List is empty\n");
  }
  else if(head->link == NULL)
  {
    free(head);
    head = NULL;
  }
    else
    {
      struct node *temp = head;
      struct node *temp2 = head;

      while(temp->link!=NULL)
      {
        temp2 = temp;
        temp = temp->link;
      }

      temp2->link = NULL;
      free(temp);
      temp=NULL;
    }

    return head;

}

int main()
{
  struct node *head = malloc(sizeof(struct node));
  head -> data = 34;
  head -> link = NULL;

  struct node *current = malloc(sizeof(struct node));
  current -> data = 45;
  current -> link = NULL;


  head -> link = current;

  current = malloc(sizeof(struct node));
  current -> data = 56;
  current -> link = NULL;

  head->link->link = current;

  struct node *ptr =NULL;
  head= del_end(head);
  ptr = head;

  while(ptr!=NULL)
  {
    printf("%d\n",ptr->data);
    ptr = ptr->link;
  }

  return 0;
}


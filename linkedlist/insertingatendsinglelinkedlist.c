#include <stdio.h>
#include <stdlib.h>
struct node 
{
  int data;
  struct node *link;
};

void *add_end(struct node *head,int data)
{
  struct node *temp,*ptr;

  ptr = head;

  temp = malloc(sizeof(struct node));
  
  temp-> data = data;
  temp->link = NULL;


  while(ptr->link!= NULL)
  {
    ptr = ptr->link;
  }
  ptr->link = temp;
}
int main()
{
  struct node *ptr = NULL;

  struct node *head = malloc(sizeof(struct node));
  head -> data =56;
  head -> link = NULL;

  struct node *current = malloc(sizeof(struct node));
  current -> data = 43;
  current -> link = NULL;
  head->link = current;

  current = malloc(sizeof(struct node));
  current -> data = 98;
  head->link->link = current;


  int data = 21;
  add_end(head,data);
  ptr = head;

while(ptr != NULL)
{
  printf("%d\n",ptr->data);
  ptr = ptr-> link;
}

}

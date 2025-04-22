#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


struct node
{
  int data;
  struct node *link;
};
struct node* createnode(int data)
{
  struct node *newnode=malloc(sizeof(struct node));
  newnode->data=data;
  newnode->link= NULL;
return newnode;
}
struct node *reverse(struct node *head)
{
  struct node *prev = NULL;
  struct node *next = NULL;

  while(head != NULL)
  {
    next = head -> link;
    head -> link = prev;
    prev = head;
    head = next;
  }

  head = prev;
  return head;
}


int main()
{
  
  struct node *head = NULL;
  struct node *ptr = NULL;

  head = malloc(sizeof(struct node));
  head -> data = 21;
  head -> link =NULL;


  struct node *current = malloc(sizeof(struct node));
  current -> data = 54;
  head-> link = current;



  current = malloc(sizeof(struct node));
  current -> data = 74;
  head->link->link = current;

  head = reverse(head);
  ptr = head;

  while(ptr != NULL)
  {
    printf("%d\n",ptr-> data);
    ptr = ptr-> link;
  }
}

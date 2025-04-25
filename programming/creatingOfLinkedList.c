#include <stdio.h>
#include<stdlib.h>

struct node{
    int data;
    struct node *next;
    
};

void printlist(struct node *temp){
    while(temp!=0){
        printf("%d " , temp->data);
        temp=temp->next;
        
    }
    
}

struct node *createnode(int data){
    struct node *newnode= (struct node *)malloc(sizeof(struct node));
    newnode->data=data;
    newnode->next=0;
    
    return newnode;
}
int main()
{
  struct node *head=createnode(3);
  struct node *second = createnode(4);
  struct node *third= createnode(5);
  
  head->next= second;
  second->next=third;
  
  printlist(head);

    return 0;
}

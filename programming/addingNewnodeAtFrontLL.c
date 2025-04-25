/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

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
    printf("\n");
    
}

struct node *createnode(int data){
    struct node *newnode= (struct node *)malloc(sizeof(struct node));
    newnode->data=data;
    newnode->next=0;
    
    return newnode;
}

struct node *insertatfront(struct node *head, int i){
    struct node *newnode = createnode(i);
    newnode->next=head;
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
  int a=2;
  head=insertatfront(head,a);
  printlist(head);

    return 0;
}

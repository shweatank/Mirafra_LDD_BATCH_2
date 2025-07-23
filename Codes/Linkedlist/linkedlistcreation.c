#include <stdio.h>
#include <stdlib.h>

struct node
{
    int data;
    struct node* link;
};
int main()
{
    int n,i;
    struct node *head=NULL,*newnode=NULL,*temp=NULL;
    
    printf("Enter no.of Nodes:");
    scanf("%d",&n);
    
    for(i=1;i<=n;i++)
    {
        newnode = (struct node *)malloc(sizeof(struct node));
        printf("Enter Node%d:",i);
        scanf("%d",&newnode->data);
        newnode->link = NULL;
        
        if(head == NULL)
        {
            head = newnode;
            temp = newnode;
        }
        else
        {
            temp->link = newnode;
            temp = newnode;
        }
    }
    
    printf("Linkedlist:");
    temp = head;
    while(temp!=NULL)
    {
        printf("%d->",temp->data);
        temp = temp->link;
    }
    
    printf("NULL\n");
}

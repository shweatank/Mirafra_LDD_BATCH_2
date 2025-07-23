#include <stdio.h>
#include <stdlib.h>


struct node 
{
    int data;
    struct node* link;
};
struct node* Insertatbeg(struct node *head,int value)
{
     struct node *begnode = (struct node *)malloc(sizeof(struct node));
         begnode->data = value;
         begnode->link = head;
     
     return begnode;
}

void printData(struct node *head)
{
    struct node *temp = NULL;
    temp = head;
    while(temp!=NULL)
    {
        printf("%d->",temp->data);
        temp= temp->link;
    }
    printf("NULL\n");
}

int main()
{
    int i,n,value;
    struct node *head = NULL,*temp = NULL,*newnode = NULL;
    printf("Enter no.of nodes:");
    scanf("%d",&n);
    
    for(i=1;i<=n;i++)
    {
        newnode = (struct node *)malloc(sizeof(struct node));
        printf("Enter node %d:",i);
        scanf("%d",&newnode->data);
        
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
    
    
    printData(head);
    
    printf("Enter the value to add node at beg:");
    scanf("%d",&value);
    
    head = Insertatbeg(head,value);
    printData(head);

    
}

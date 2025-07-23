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

struct node* Insertatend(struct node *head,int value)
{
     struct node *endnode = (struct node *)malloc(sizeof(struct node));
         endnode->data = value;
         endnode->link = NULL;
         
         if(head == NULL)
         {
             head = endnode;
         }
         else
         {
            struct node *temp = head;
            while(temp->link!=NULL)
            {
                temp = temp->link;
            }
            temp->link = endnode;
         }
     
     return head;
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
    int i,n,value1,value2;
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
    scanf("%d",&value1);
    
    head = Insertatbeg(head,value1);
    printData(head);
    
    printf("Enter the value to add node at END:");
    scanf("%d",&value2);
    
    head = Insertatend(head,value2);
    printData(head);
    
}

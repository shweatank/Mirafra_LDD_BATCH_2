
LINKED LIST
1)//creating a LL using loops
#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* link;
};

int main() {
    struct Node* head = NULL;
    struct Node* current = NULL;
    struct Node* newNode = NULL;

    int n, value;
    printf("Enter number of nodes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        printf("Enter value for node %d: ", i + 1);
        scanf("%d", &value);

        // Create a new node
        newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->data = value;
        newNode->link = NULL;

        if (head == NULL) {
            // First node becomes the head
            head = newNode;
            current = head;
        } else {
            // Link the new node and move current
            current->link = newNode;
            current = newNode;
        }
    }

    // Print the linked list
    printf("Linked List:\n");
    struct Node* temp = head;
    while (temp != NULL) {
        printf("%d -> ", temp->data);
        temp = temp->link;
    }
    printf("NULL\n");

    return 0;
}

VERY IMP
2)Reversing single LL
#include <stdio.h>
#include <stdlib.h>
struct Node* reverse(struct Node *head);
struct Node {
    int data;
    struct Node* link;
};

int main() {
    struct Node* head = NULL;
    struct Node* current = NULL;
    struct Node* newNode = NULL;

    int n, value;
    printf("Enter number of nodes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {                        
        printf("Enter value for node %d: ", i + 1);
        scanf("%d", &value);

        // Create a new node
        newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->data = value;
        newNode->link = NULL;

        if (head == NULL) {
            // First node becomes the head
            head = newNode;
            current = head;
        } else {
            // Link the new node and move current
            current->link = newNode;
            current = newNode;
        }
    }

    // Print the linked list
    printf("Linked List:\n");
    struct Node* temp = head;
    while (temp != NULL) {
        printf("%d -> ", temp->data);
        temp = temp->link;
    }
    printf("NULL\n");
    head=reverse(head);
   
     printf("Reversed Linked List:\n");
     temp = head;
    while (temp!= NULL) {
        printf("%d -> ", temp->data);
        temp = temp->link;
    }
    printf("NULL\n");

    return 0;
}
struct Node* reverse(struct Node *head)
{
    struct Node *current=head;
    struct Node* prev=NULL;
    struct Node *next=NULL;
    while(current!=NULL)
    {
        next=current->link;
        current->link=prev;
        prev=current;
        current=next;
    }
    head=prev;
    return head;
    
}
//hardcoded values
#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* link;
};

// Function to reverse the linked list
struct Node* reverseList(struct Node* head) {
    struct Node* prev = NULL;
    struct Node* current = head;
    struct Node* next = NULL;

    while (current != NULL) {
        next = current->link;
        current->link = prev;
        prev = current;
        current = next;
    }

    return prev;
}

int main() {
    struct Node* head = NULL;
    struct Node* current = NULL;
    struct Node* newNode = NULL;

    int values[] = {10, 20, 30, 40};  // 💡 Hardcoded values
    int n = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < n; i++) {
        newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->data = values[i];
        newNode->link = NULL;

        if (head == NULL) {
            head = newNode;
            current = head;
        } else {
            current->link = newNode;
            current = newNode;
        }
    }

    // Print original list
    printf("Original Linked List:\n");
    current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->link;
    }
    printf("NULL\n");

    // Reverse the list
    head = reverseList(head);

    // Print reversed list
    printf("\nReversed Linked List:\n");
    current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->link;
    }
    printf("NULL\n");

    return 0;
}

//LINKED LIST ALL OPERATIONS

#include<stdio.h>
#include<stdlib.h>
struct node 
{
    int data;
    struct node *link;
};

void createnode(struct node **head)
{
    int n,value;
    printf("Enter the no of nodes ");
    scanf("%d",&n);
    struct node *current=NULL;
    struct node *newnode=NULL;
    for(int i=0;i<n;i++)
    {
        printf("Enter the value in %d node ",i+1);
        scanf("%d",&value);
        struct node *newnode=malloc(sizeof(struct node));
        newnode->data=value;
        newnode->link=NULL;
        
        if(*head==NULL)
        {
          *head=newnode;
           current=*head;
         }   
         else
        {
           current->link=newnode;
           current=newnode;
         }
    }
}
void printlist(struct node *head)
{
    struct node *temp=head;
    while(temp!=NULL)
    {
        printf("%d ",temp->data);
        temp=temp->link;
    }
    printf("NULL\n");
}
struct node* reverselist(struct node *head)
{
    struct node *next;
    struct node *prev=NULL;
    struct node *current=head;
    while(current!=NULL)
    {
        next=current->link;
        current->link=prev;
        prev=current;
        current=next;
    }
    return prev;
}
struct node* insertatfront(struct node **head,int data)
{
    struct node *newnode=malloc(sizeof(struct node));
    newnode->data=data;
    newnode->link=*head;
    *head=newnode;
    return *head;
    
}
void insertatend(struct node **head,int data)
{
    struct node *newnode=malloc(sizeof(struct node));
    newnode->data=data;
    newnode->link=NULL;
    if (*head == NULL) {
        *head = newnode;
        return;
    }
    struct node *temp=*head;
    while(temp->link!=NULL)
    {
        temp=temp->link;
    }
    temp->link=newnode;
}
void insertatpos(struct node **head,int data,int pos)
{
    struct node *newnode=malloc(sizeof(struct node));
    newnode->data=data;
    newnode->link=NULL;
    struct node *temp=*head;
    if(pos==1)
    {
        insertatfront(head,data);
        return ;
    }
    else
    {
        for(int i=1;i<pos-1&&temp!=NULL;i++)
        {
            temp=temp->link;
        }
        
        if (temp == NULL) {
        printf("Position out of range\n");
        free(newnode);
        return;
        }
        newnode->link=temp->link;
        temp->link=newnode;
    }
}
struct node* deleteatfront(struct node  **head)
{
    /*
    if(*head==NULL)
    {
        return;
    }*/
    struct node *temp=*head;
    *head=temp->link;
    free(temp);
    return *head;
}
void deleteatend(struct node **head)
{
    if(*head==NULL)
    {
        return;
    }
    if((*head)->link==NULL) //1 node
    {
        free(*head);
        *head=NULL;
        return;
    }
    struct node *prev=NULL;
    struct node *current=*head;
    while(current->link!=NULL)
    {
        prev=current;
        current=current->link;
    }
    prev->link=NULL;
    free(current);
}
void deleteatpos(struct node **head,int pos)
{
    if(*head==NULL)
    {
        return;
    }
    if(pos==1)
    {
        deleteatfront(head);
        return;
    }
    struct node*temp=*head;
    for(int i=1;i<pos-1&&temp->link!=NULL;i++)
    {
        temp=temp->link;
    }
    struct node*todelete=temp->link;
    temp->link=todelete->link;
    free(todelete);
}
void sortlist(struct node *head)
{
    struct node *i=head;
    while(i!=NULL)
    {
        struct node *j=i->link;
        while(j!=NULL)
        {
            if(i->data>j->data)
            {
                int temp=i->data;
                i->data=j->data;
                j->data=temp;
            }
            j=j->link;
        }
        i=i->link;
    }
}
void findmiddle(struct node *head)
{
    struct node *fast=head;
    struct node *slow=head;
    while(fast!=NULL && fast->link!=NULL)
    {
        slow=slow->link;
        fast=fast->link->link;
    }
    if(slow!=NULL)
    {
        printf("Middle is %d\n ",slow->data);
    }
}
int main()
{
    int n;
    struct node *head=NULL;
    createnode(&head);
    printlist(head);
    //head=reverselist(head);
   // printlist(head);
    head=insertatfront(&head,5);
    printlist(head);
    insertatend(&head,45);
    printlist(head);
    insertatpos(&head,25,4);
    printlist(head);
    head=deleteatfront(&head);
    printlist(head);
    deleteatend(&head);
    printlist(head);
    deleteatpos(&head,3);
    printlist(head);
    sortlist(head);
    printlist(head);
    findmiddle(head);
    printlist(head);
    
}
  

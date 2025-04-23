#include<stdio.h>

struct node
 {
        int num;
          struct node* next;
};
void insertNode(node *head,int val)
 {
    node* newNode = new node(val);
    if(head == NULL)
     {
        head = newNode;
        return;
    }
    node* temp = head;
    while(temp->next != NULL) temp = temp->next;
    temp->next = newNode;
}
node* getNode(slist head,int val) 
{
    while(head->num != val) 
    head = head->next;
    
    return head;
}
void deleteNode(node* t) 
{
    t->num = t->next->num;
    t->next = t->next->next;
    return;
}
void printList(node* head) 
{
    while(head->next != NULL) 
    {
    printf("%d->",head->num);
        head = head->next;
    }
   printf("%d\n",head->num);
}

int main() {
    node* head = NULL;
    insertNode(head,1);
    insertNode(head,4);
    insertNode(head,2);
    insertNode(head,3);
    printf("Given Linked List:\n");
    printList(head);
    node* t = getNode(head,2);
    printf("Linked List after deletion:\n");
    printList(head);
    return 0;
}


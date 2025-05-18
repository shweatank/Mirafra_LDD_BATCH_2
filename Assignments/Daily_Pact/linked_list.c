#include <stdio.h>
#include <stdlib.h>


struct Node{
    int data;
    struct Node *next;
};

int sum = 0;
struct Node* creatnode(int value){
    struct Node* head = (struct Node*)malloc(sizeof(struct Node));
    head->data=value;
    head->next=NULL;
    
    return head;
} 

void append(struct Node **head_ref,struct Node **tail_ref,int value){
struct Node *new_node = creatnode(value);
if(*head_ref == NULL){
    *head_ref = new_node;
    *tail_ref = new_node;
}else{
    (*tail_ref)->next=new_node;
    *tail_ref =new_node;
}
}


void printlist(struct Node *head){
    struct Node *current = head;
    while(current != NULL){
        sum += current->data;
        printf("%d->",current->data);
        current = current->next;
    }
    printf("NULL\n");
    printf("Sum:%d\n",sum);
}

void rev(struct Node** head_ref) {
    struct Node* prev = NULL;
    struct Node* current = *head_ref;
    struct Node* next = NULL;

    while (current != NULL) {
        next = current->next;    
        current->next = prev;     
        prev = current;         
        current = next;         
    }

    *head_ref = prev;             
}

void push(struct Node** head_ref, int value){
    struct Node* new_node = creatnode(value);
    new_node->data=value;
    new_node->next=*head_ref;
    *head_ref = new_node;
}

int main(){
    struct Node *head = NULL;
    struct Node *tail = NULL;
    int n,value;
    printf("enter number of nodes: ");
    scanf("%d",&n);
    
    for(int i=1;i<=n;i++){
        printf("enter data for node %d:\n ",i);
        scanf("%d",&value);
        append(&head,&tail,value);
    }
    
    printf("Original list:\n");
    printlist(head);

    rev(&head);

    printf("Reversed list:\n");
    sum = 0;
    printlist(head);
    
    push(&head,20);
    printf("front add:");   
    printlist(head);
    
    
    
}






































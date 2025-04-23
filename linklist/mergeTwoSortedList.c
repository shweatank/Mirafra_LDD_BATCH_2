#include<stdio.h>
#include<stdlib.h>
 struct ListNode {
      int val;
     struct ListNode *next;
 };

void insertatend(struct ListNode**, int);
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2);
int main(){

   int data, size,i;
   struct ListNode * list1 = NULL, *list2 = NULL,* head = NULL,*ptr;
   printf("enter the size of the first list\n");
   scanf("%d",&size);
   printf("enter the elemets \n");
   for( i = 0; i < size ;i++){
      scanf("%d",&data);
      insertatend(&list1,data);
   }
   printf("enter the size of the second list\n");
   scanf("%d",&size);
    
   printf("enter the elemets \n");
   for( i = 0; i < size ;i++){
      scanf("%d",&data);
      insertatend(&list2,data);
   }
    
     head = mergeTwoLists(list1,list2);
     if(head == NULL){
	   printf("no elements");
     } else{
        ptr = head;
	while(ptr->next != NULL){
	    printf("%d->",ptr->val);
	    ptr = ptr->next;
	}
	printf("%d",ptr->val);
     }
  return 0;
}
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2) {
    struct ListNode* head = NULL;
    if(list1 == NULL) {
        return list2;
    }
    if(list2 == NULL){
       return list1;
    }
    while((list1 != NULL) && (list2 != NULL)) {
        if((list1->val) <= (list2->val)) {
            insertatend(&head, list1->val);
            list1 = list1->next;
        } else {
            insertatend(&head, list2->val);
            list2 = list2->next;
        }
    }
    while(list1 != NULL){
        insertatend(&head, list1->val);
        list1 = list1->next;
    }
    while(list2 != NULL){
        insertatend(&head, list2->val);
        list2 = list2->next;
    }
    return head;
}

void insertatend(struct ListNode** head, int val) {
    struct ListNode* ptr = *head;
    struct ListNode* newnode = malloc(sizeof(struct ListNode));
    if (newnode == NULL) {
        return;
    }
    newnode->val = val;
    newnode->next = NULL;
    if (*head == NULL) {
        *head = newnode;
    } else {
        ptr = *head;
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = newnode;
    }
}


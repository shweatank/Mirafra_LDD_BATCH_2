#include<stdio.h>
#include<stdlib.h>

typedef struct node{
	int data;
	struct node* next;
}ListNode;

ListNode* removeNthFromEnd( ListNode* head, int n);
void insertatEnd(ListNode** head, int data);
int main(){
        int size, data = 0,num;
	ListNode* head = NULL,*ptr = NULL;
	printf("enter the size of the list\n ");
	scanf("%d",&size);
	printf("enter the elements \n");
	for(int i = 0; i < size; i++){
		scanf("%d",&data);
		insertatEnd(&head, data);
	}
        printf("enter the number which you want to delete\n");
	scanf("%d",&num);
	head =  removeNthFromEnd(head, num);
	printf("\n output is  ");
	if(head != NULL){
		ptr = head;
		while(ptr->next != NULL){
			printf("%d-> ",ptr->data);
			ptr = ptr->next;
		}
		printf("%d\n",ptr->data);
	}

	return 0;
}






ListNode* removeNthFromEnd( ListNode* head, int n) {

	if(head == NULL){
		return NULL;
	}
	ListNode* temp = head;
	ListNode* prev = head;
	int totalLength = 1,length = 0;
	while(temp->next != NULL){
		totalLength++;
		temp = temp->next;
	}
	length = totalLength - n;
	if(length == 0 && (head->next == NULL)){
		head = NULL;
		return head;
	}
	else if(length == 0 && (head->next != NULL)){
		head =  head->next;
	}
	temp =  head;
	while(length){
		prev = temp;
		temp = temp->next;
		length--;
	}
	prev->next = temp->next;

	return head;
}



void insertatEnd(ListNode** head, int data){

     ListNode* temp = *head;
     ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
     if(newNode == NULL){
      return;
     }
     newNode->data = data;
     newNode->next =  NULL;
     if(*head == NULL){
	 (*head) = newNode;
     } else{
         temp =*head;
	 while(temp->next != NULL){
	      temp =  temp->next;
	 }
	 temp->next = newNode;
     
     }
}

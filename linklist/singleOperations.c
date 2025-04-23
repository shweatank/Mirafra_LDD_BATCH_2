#include"singleOperations.h"

node*  create_node(int data)
{
	node * newNode = malloc(sizeof(struct node));

	if(newNode == NULL){
		return NULL;
	} 

	newNode->data = data;
	newNode->link = NULL;
	return newNode;
}

int  insert_atend(node** head,int data){

	node * newNode = NULL;
	node * temp = NULL;
	newNode = create_node(data);
	if(newNode == NULL){

		return FAIL;
	}

	if(*head == NULL){

		*head = newNode;
		(*head)->data = newNode->data;
		(*head)->link = NULL;
	}else {

		temp = * head;
		while(temp->link != NULL){

			temp = temp->link;
		}
		temp->link = newNode;

	}
	return PASS;
}

int insert_atbegin(node** head, int data){

	node * newNode = NULL;
	node * temp = NULL;
	newNode = create_node(data);
	if(newNode == NULL){

		return FAIL;
	}

	if(*head == NULL){

		*head = newNode;
		(*head)->data = newNode->data;
		(*head)->link = NULL;
	}else {

		newNode->link = *head;
		*head = newNode;
	}
	return PASS;
}

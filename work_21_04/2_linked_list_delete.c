#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <wait.h>


typedef struct Linked{
    int data;
    struct Linked *next;
}Linked_List;


void insert_node(Linked_List **head, int data);
void delete_node(Linked_List **head, int data);
void print_list(Linked_List *head); 

int main()
{
    Linked_List *ll = NULL;
    short option;

    while(1)
    {
	printf("Enter : \n1.Insert Node\n2.Delete Node\n3.Print List\n");
	scanf("%hd",&option);;
	int data;
	switch(option) 
	{
	    case 1: 
		printf("Enter Data : ");
		scanf("%d",&data);
		insert_node(&ll,data);
		break;
	    case 2: 
		printf("Enter Data : ");
		scanf("%d",&data);
		delete_node(&ll,data);
		break;
	    case 3:
		print_list(ll);
		break;
dafault:
		write(STDOUT_FILENO,"Invalid Choice\n",17);

	}
    }
    return 0;
}

void insert_node(Linked_List **head, int data)
{
    Linked_List *new = malloc(sizeof(Linked_List));
    new->next = NULL;
    new-> data = data;

    if(*head == NULL){
	*head = new;
	return;
    }
    Linked_List *temp = *head;
    while(temp->next){
	temp = temp->next;
    }
    temp->next = new;
    return ;
}

void delete_node(Linked_List **head, int data)
{
    if(*head == NULL) {
	printf("Empty List\n");
	return;
    }
    Linked_List *temp = *head, *prev = *head;

    while(temp) 
    {

	if(temp->data == data) {
	    if(temp == prev) {
		*head = temp->next;
		printf("Yes\n");
		free(temp);
		return;
	    }
	    prev->next = temp->next;
	    free(temp);
	    return;
	}
	prev = temp;
	temp = temp->next;
    }

    write(STDOUT_FILENO,"Data not present in List\n",26);
}



void print_list(Linked_List *head) {
    while(head) {
	printf("%d -> ",head->data);
	head = head->next;
    }
    printf("NULL\n\n");
}

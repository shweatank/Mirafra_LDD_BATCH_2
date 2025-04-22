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
void print_list(Linked_List *head) {
    while(head) {
	printf("%d -> ",head->data);
	head = head->next;
    }
    printf("NULL\n\n");
}


int main()
{
    Linked_List *ll = NULL;
    short option;
    
    while(1)
    {
	printf("Enter 1 -> To enter data\nEnter 2 -> to print Linked List\n");
	scanf("%hd",&option);
	if(option == 1) {
	    printf("Enter Data : ");
	    int data;
	    scanf("%d",&data);
	    insert_node(&ll,data);
	} else if(option == 2){
	    print_list(ll);
	} else {
	    write(STDOUT_FILENO,"Invalid Choice.\n",17);
	}
    }



    //void insert_node(&ll
    return 0;
}


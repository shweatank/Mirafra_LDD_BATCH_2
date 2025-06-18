#include <stdio.h>
#include <stdlib.h>

#define success 0
#define failure -1

typedef struct node{
	int data;
	struct node *link;
}slist;

int insert_at_last(slist **head, int ndata)
{
    slist *nnode = (slist *)malloc(sizeof(slist));
	if(nnode == NULL)
	{
		return failure;
	}
	nnode->data = ndata;
	nnode->link = NULL;
	
	if(*head == NULL)
	{
		*head = nnode;
		return success;
	}
    slist *temp = *head;
    while(temp->link != NULL)
    {
        temp = temp->link;
    }
    temp->link = nnode;
    return success;
}
int printlist(slist *head)
{
    if(head == NULL)
    {
        return failure;
    }
    // printf("Head ->");
    slist *temp = head;
    while(temp != NULL)
    {
        printf("%d ->", temp->data);
        temp = temp->link;
    }
    printf("NULL\n");
    return success;
}

int merge_sort(slist **head1, slist **head2, slist **temp_head)
{
    slist dummy;
    slist *temp = &dummy;
    dummy.link = NULL;
    
    while(*head1 && *head2)
    {
        if((*head1)->data < (*head2)->data)
        {
            temp->link = *head1;
            *head1 = (*head1)->link;
        }
        else
        {
            temp->link = *head2;
            *head2 = (*head2)->link;
        }
        temp = temp->link;
    }
    
    if(*head1 != NULL)
    {
       temp->link = *head1; 
    }
    
    if(*head2 != NULL)
    {
        temp->link = *head2;
    }
    
    *temp_head = dummy.link;
    return success;
}
int main()
{
    slist *head1 = NULL; 
    slist *head2 = NULL;
    slist *new_head = NULL;
    insert_at_last(&head1, 10);
    insert_at_last(&head1, 20);
    insert_at_last(&head1, 30);
    insert_at_last(&head2, 40);
    insert_at_last(&head2, 50);
    insert_at_last(&head2, 60);
    printf("HEAD1 ->");
    printlist(head1);
    printf("HEAD2 ->");
    printlist(head2);
    
    merge_sort(&head1, &head2, &new_head);
   printf("After merge sort the new head is ");
   printlist(new_head);
    
}


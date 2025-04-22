#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct node {
    struct node *next;
    int data;
}slist;

#define success 0
#define failure -1

int insert_at_last_circuilar(slist **head, int ndata)
{
    slist *nnode = (slist *)malloc(sizeof(slist));
    if(nnode == NULL)
    {
        return failure;
    }
    nnode->data = ndata;
    nnode->next = NULL;
    
    if(*head == NULL)
    {
        *head = nnode;
        nnode->next = *head;
        return success;
    }
    slist *temp = *head;
    while(temp->next != *head)
    {
      temp = temp->next;   
    }
    temp->next = nnode;
    nnode->next = *head;
    return success;
}

int print_list(slist *head)
{
    if(head == NULL)
    {
        return failure;
    }
    
    slist *temp = head;
    int s =1;
        printf("(HEAD)");
    while(temp != head || s)
    {
        s=0;
        printf("%d ->", temp->data);
        temp = temp->next;
    }
    printf("(HEAD)\n");
}
int main()
{
    slist *head =NULL;
    insert_at_last_circuilar(&head, 10);
     insert_at_last_circuilar(&head, 20);
      insert_at_last_circuilar(&head, 30);
       insert_at_last_circuilar(&head, 40);
        insert_at_last_circuilar(&head, 50);
    
    print_list(head);

    return 0;
}

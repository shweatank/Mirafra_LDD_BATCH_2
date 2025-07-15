#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node{
    int data;
    struct node *link;
}slist ;

#define success 0
#define failure -1

//removing the duplicates
int remove_duplicates(slist **head)
{
    if(*head == NULL)
    {
        return failure;
    }
    slist *current = *head;
    while(current != NULL && current->link != NULL)
    {
        slist *next = current;
        
        while(next->link != NULL)
        {
            if(current->data == next->link->data)
            {
                slist* temp = next->link;
                next->link = temp->link;
                free(temp);
            }
            next = next->link;
        }
        current = current->link;
    }
    return success;
}

//sorting the list in ascending order
int sl_sort(slist **head)
{
    if(*head == NULL  || (*head)->link == NULL)
    {
        return failure;
    }
    slist *current = *head;
    slist *next = NULL;
    while(current != NULL)
    {
        next = current->link;
        while(next != NULL)
        {
            if(current->data > next->data)
            {
                int temp = current->data;
                current->data = next->data;
                next->data = temp;
            }
            next = next->link;
            
        }
        current = current->link;
    }
    return success;
}

//reverse the linkedlist in iteration method
int reverce_ll_iter(slist **head)
{
    if(*head == NULL)
    {
        return failure;
    }
    
    slist *prev = NULL;
    slist *current = *head;
    slist *next = NULL;
    while(current != NULL)
    {
        next = current->link;
        current->link = prev;
        prev = current;
        current = next;
    }
    *head = prev;
    return success;
}

//get the nth last bit via position from last
int get_nth_last(slist *head, int pos, int *data)
{
    if(head == NULL)
    {
        return failure;
    }
    if(pos <=0)
    {
        return failure;
    }
    slist *f1 = head;
    slist *f2 = head;
    int count = 0;
    while(count < pos)
    {
        if(f1 == NULL)
        {
            return failure;
        }
        f1 = f1->link;
        count++;
    }
    
    while(f1 != NULL)
    {
        f1 = f1->link;
        f2 = f2->link;
    }
    *data = f2->data;
    return success;
}

//find the middle node
int find_mid(slist *head, int *mid)
{
    if(head == NULL)
    {
        return failure;
    }
    
    slist *f1 = head;
    slist *f2 = head;
    while(f1 != NULL && f1->link != NULL)
    {
        f1 = f1->link->link;
        f2 = f2->link;
    }
    *mid = f2->data;
    return success;
}

//find the appropriate node in the list
int find_node(slist *head, int key)
{
    int count =1;
    if(head == NULL)
    {
        return failure;
    }
    slist *temp = head;
    while(temp != NULL)
    {
        if(temp->data == key)
        {
           return count; 
        }
        else
        {
            count++;
            temp = temp->link;
        }
    }
}

//delete total linkedlist 
int delete_list(slist **head)
{
    if(*head == NULL)
    {
        return failure;
    }
    
    slist *temp = *head;
    while(*head != NULL)
    {
       *head = temp->link;
       free(temp);
       temp = *head;
        
    }
    return success;
}

//delete the first node in the list
int delete_atfirst(slist **head)
{
    if(*head == NULL)
    {
        return failure;
    }
    if((*head)->link == NULL)
    {
        free(*head);
        *head = NULL;
        return success;
    }
    slist *temp = *head;
    *head = temp->link;
    free(temp);
    return success;
}

//delete the last node in the list
int delete_atlast(slist **head)
{
    if(*head == NULL)
    {
        return failure;
    }
    slist *temp = *head;
    slist *prev  = NULL;
    while(temp->link != NULL)
    {
        prev = temp;
        temp = temp->link;
    }
    prev->link= NULL;
    free(temp);
    return success;
}

//print the list
int Printlist(slist *head)
{
    if(head == NULL)
    {
        return failure;
    }
    
    printf("Head -> ");
    slist *temp = head;
    while(temp != NULL)
    {
        printf("%d -> ", temp->data);
        temp = temp->link;
    }
    printf("NULL \n");
    return success;
    
}

//insert a node at last of the list
int Insert_atlast(slist **head, int ndata)
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
    temp->link= nnode;
    return success;
}

//insert a node at first of the list
int Insert_atfirst(slist **head, int ndata)
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
    nnode->link = *head;
    *head = nnode;
    return success;
}

slist *recursion_revrse(slist *current, slist *prev)
{
    if(current == NULL)
    {
        return prev;
    }
    slist *next = current->link;
    current->link = prev;
    recursion_revrse(next, current);
}
int reverse_sll_recursion(slist **head)
{
    if(*head ==NULL)
    {
        return failure;
    }
    *head = recursion_revrse(*head, NULL);
}

int main()
{
    int data, option,key;
    slist *head = NULL;
    
    printf("\n1.Insert at first\n2.Insert at last\n3.Printlist\n4.delete at last\n5.delete at first\n6.delete list\n7.Find node\n8.find mid\n9.get nth last node\n");
    
    while(1)
    {
        printf("\nEnter your choice!\n");
        scanf("%d", &option);
        switch(option)
        {
            case 1:
            printf("Enter your Data!\n");
            scanf("%d", &data);
            if(Insert_atfirst(&head, data) != 0)
            {
                printf("Insert at first error\n");
            }
            break;
               case 2:
            printf("Enter your Data!\n");
            scanf("%d", &data);
            if(Insert_atlast(&head, data) != 0)
            {
                printf("Insert at last error\n");
            }
            break;
               case 3:
            if(Printlist(head) != 0)
            {
                printf("printlist error\n");
            }
            break;
               case 4:
            if(delete_atlast(&head) != 0)
            {
                printf("delete at last error\n");
            }
            break;
               case 5:
            if(delete_atfirst(&head) != 0)
            {
                printf("delete at first error\n");
            }
            break;
               case 6:
            if(delete_list(&head) != 0)
            {
                printf("delete list error\n");
            }
            break;
               case 7:
            printf("Enter your key!\n");
            scanf("%d", &key);
            int count = 0;
            if((count = find_node(head, key)) != 0)
            {
                printf("key is found at the position %d", count);
            }
            else
            {
                printf("key is not found\n");
            }
            break;
            case 8:
            int mid =0;
            if(find_mid(head, &mid) != -1)
            {
                printf("The mid node is %d", mid);
            }
            break;
            
            case 9:
            int pos;
            printf("Enter the position of the node from the last \n");
            scanf("%d", &pos);
            if(get_nth_last(head, pos, &data) != -1)
            {
                printf("the %d pos of nth last element data is %d", pos, data);
            }
            break;
            
            case 10:
            if(reverce_ll_iter(&head) != -1)
            {
                printf("The linked list is reversed through iteration method\n");
                
            }
            break;
            case 11:
            if(reverse_sll_recursion(&head) != -1)
            {
            printf("The linked list is reversed through recursion method\n");
            }
            case 12:
            if(sl_sort(&head) != -1)
            {
                printf("The linked list is sorted\n");
            }
            break;
            case 13:
            if(remove_duplicates(&head) != -1)
            {
                printf("The linked list duplicates are removed\n");
            }
            break;
        }
    }
}


/*
 ============================================================================
 Name        : doubly_link_list.c
 Author      : shubham
 Version     :
 Copyright   : learning the code
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include<stdio.h>
#include<stdlib.h>
struct node
{
    struct node *p;
    int data;
    struct node *n;
};
int insert_end(struct node **,int);
int display(struct node *);
int insert_beg(struct node **,int);
int delete_beg(struct node **);
int delete_end(struct node **);
int insert_pos(struct node **,int,int);
int delete_pos(struct node **,int);

int main()
{
    int ele,choice,pos;
    struct node *head=NULL;
    while(1)
    {
        printf("***************************\n");
        printf("1 for insert element at end\n");
        printf("2 for display the all element\n");
        printf("3 for insert element at the start\n");
        printf("4 for delete at begaing\n");
        printf("5 for delete at end\n");
        printf("6 for insert at position\n");
        printf("7 for delete a position\n");
        printf("8 for exit\n");
        printf("default: for try again\n");
        printf("***************************\n");
        printf("enter the choice\n");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:printf("enter the element\n");
                   scanf("%d",&ele);
                   insert_end(&head,ele);
                   break;
            case 2:display(head);
                   break;
            case 3:printf("enter the element\n");
                   scanf("%d",&ele);
                   insert_beg(&head,ele);
                   break;
            case 4:delete_beg(&head);
                   break;
            case 5:delete_end(&head);
                   break;
            case 6:printf("enter the element\n");
                   scanf("%d",&ele);
                   printf("enter the postion\n");
                   scanf("%d",&pos);
                   insert_pos(&head,ele,pos);
                   break;
            case 7:printf("enter the postion\n");
                   scanf("%d",&pos);
                   delete_pos(&head,pos);
                   break;
            case 8:exit(0);
                   break;
            default:printf("try again\n");
                   break;
        }
    }
    return 0;
}
int insert_end(struct node **p,int ele)
{
    struct node *temp,*t;
    t=*p;
    temp=(struct node *)malloc(sizeof(struct node));
    temp->p=NULL;
    temp->data=ele;
    temp->n=NULL;
    if(*p==NULL)
    {
        *p=temp;
    }
    else{
        while(t->n!=NULL)
        {
            t=t->n;
        }
        t->n=temp;
        temp->p=t;
    }
}
int display(struct node *q)
{
    while(q!=NULL)
    {
        printf(">>%d\t",q->data);
        q=q->n;
    }
    printf("\n");
}
int insert_beg(struct node **p,int ele)
{
    struct node *temp;
    temp=(struct node *)malloc(sizeof(struct node));
    temp->p=NULL;
    temp->data=ele;
    temp->n=NULL;
    if(*p==NULL)
    {
        *p=temp;
    }
    else
    {
        temp->n=*p;
        *p=temp;
    }
    display(*p);
}
int delete_beg(struct node **p)
{
    struct node *t;
    t=*p;
    if(*p==NULL)
    {
        printf("no nodes are aviliable\n");
    }
    else
    {
        *p=t->n;
        free(t);
    }
}
int delete_end(struct node **p)
{
    struct node *t1,*t2;
    t1=*p;
    t2=NULL;
    if(*p==NULL)
    printf("no nodes are aviliable\n");
    else{
        while(t1->n!=NULL)
        {
            t2=t1;
            t1=t1->n;
        }
        t2->n=NULL;
        free(t1);
    }
    display(*p);
}
int insert_pos(struct node **p,int ele,int pos)
{
    struct node *temp,*t;
    t=*p;
    int i=1;
    temp=(struct node *)malloc(sizeof(struct node));
    temp->p=NULL;
    temp->data=ele;
    temp->n=NULL;
    if(pos==1)
    {
        insert_beg(p,ele);
    }
    else{
        while(i<pos-1)
        {
          t=t->n;
          i++;
        }
        temp->p=t;
        temp->n=t->n;
        t->n->p=temp;
        t->n=temp;
    }
    display(*p);
}
int delete_pos(struct node **p,int pos)
{

    struct node *t1,*t2;
    t1=*p;
    t2=NULL;
    int i=1;
    if(pos==1)
    delete_beg(p);
    else{
        while(i<pos)
        {
            t2=t1;
            t1=t1->n;
            i++;
        }
        t2->n=t1->n;
        t1->n->p=t2;
        free(t1);
    }

}

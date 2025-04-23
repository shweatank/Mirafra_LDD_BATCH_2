/*
 ============================================================================
 Name        : circular_singly_ll.c
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
    int data;
    struct node *n;
};
void insert_end(struct node **,int);
void display(struct node *);
void insert_beg(struct node **,int);
int count(struct node  *);
void delete_beg(struct node **);
void delete_end(struct node **);
void insert_pos(struct node **,int,int);
void delete_pos(struct node **,int );


int main()
{
    int ele,choice,pos;
    struct node *head=NULL;
    while(1)
    {
        printf("***************************\n");
        printf("1 for insert element at ending\n");
        printf("2 for display the all element\n");
        printf("3 for insert element at the beging\n");
        printf("4 for delete at begaing\n");
        printf("5 for delete at end\n");
        printf("6 for insert at position\n");
        printf("7 for delete a position\n");
        printf("8 for count the element\n");
        printf("9 for exit\n");
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
            case 8:count(head);
                   printf("count=%d\n",count(head));
                   break;
            case 9:exit(0);
                   break;
            default:printf("try again\n");
                   break;
        }
    }
    return 0;
}
void insert_end(struct node **p,int ele)
{
    struct node *temp,*t;
    t=*p;
    temp=(struct node *)malloc(sizeof(struct node));
    temp->data=ele;
    temp->n=NULL;
    if(*p==NULL)
    {
        *p=temp;
	temp->n=*p;
    }
    else{
    	while(t->n!=(*p))
    	{
    		t=t->n;
    	}
    	t->n=temp;
    	temp->n=*p;
    }
}
void display(struct node *q)
{
    struct node *t;
    t=q;
    if(q==NULL)
    {
    	printf("no nodes are aviliable\n");
    }
    else{
    while(t->n!=q)
    {
           printf(">>%d\t",t->data);
           t=t->n;
    }
    printf(">>%d\n",t->data);
    }
}
void insert_beg(struct node **p,int ele)
{
    struct node *temp,*t;
    t=*p;
    temp=(struct node *)malloc(sizeof(struct node));
    temp->data=ele;
    temp->n=NULL;
    if(*p==NULL)
    {
        *p=temp;
        temp->n=*p;
    }
    else{
    	while(t->n!=(*p))
    	{
    		t=t->n;
    	}
    	t->n=temp;
    	temp->n=*p;
        *p=temp;
    }
}
int count(struct node *p)
{
       struct node *t;
       t=p;
       int count=0;
       while(t->n!=p)
       {
              count++;
              t=t->n;
       }
       count++;
       return count;
}

void delete_beg(struct node **p)
{
	struct node *t1,*t2;
    t1=*p;
    t2=*p;
    if(t1->n==(*p))
    {
    	*p=NULL;
    	free(t1);
    }
    else{
        while(t1->n!=(*p))
       {
    	t1=t1->n;
       }
        t1->n=t2->n;
        *p=t2->n;
        free(t2);
    }
}
void delete_end(struct node **p)
{
	struct node *t1,*t2;
	    t1=*p;
	    t2=NULL;
	    if(t1->n==(*p))
	    {
	    	*p=NULL;
	    	free(t1);
	    }
	    else{
	        while(t1->n!=(*p))
	       {
	        	t2=t1;
	        	t1=t1->n;
	       }
	        t2->n=*p;
	        free(t1);
	    }
}
void insert_pos(struct node **p,int ele,int pos)
{
	struct node *t1;
	t1=*p;
	struct node *temp;
	temp=(struct node *)malloc(sizeof(struct node));
	temp->data=ele;
	temp->n=NULL;
	int i=1;
	if(pos==1)
	{
		insert_beg(p,ele);
	}
	else{
	while(i<pos-1)
	{
		t1=t1->n;
		i++;
	}
	temp->n=t1->n;
	t1->n=temp;
  }
}

void delete_pos(struct node **p,int pos)
{
	struct node *t1,*t2;
	t1=*p;
	t2=NULL;
	int i=1;
	if(pos==1)
	{
		delete_beg(p);
	}
	else{
		while(i<pos)
		{
			t2=t1;
			t1=t1->n;
			i++;
		}
		t2->n=t1->n;
		free(t1);
	}
}







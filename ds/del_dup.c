#include<stdio.h>
#include<stdlib.h>
struct st
{
		int data;
		struct st *link;
};
void add(struct st **);
void delete(struct st *);
int main()
{
		struct st *head=NULL;
		add(&head);
		delete(head);
		return 0;
}
void add(struct st **head)
{
		printf("Enter elements:\n");
		for(int i=0;i<5;i++)
		{
				int n=0;
				scanf("%d",&n);
				if(*head==NULL)
				{
						struct st *new=malloc(sizeof(struct st));
						new->data=n;
						new->link=NULL;
						*head=new;
				}
				else
				{
						struct st *temp=*head;
						while(temp->link!=NULL)
						{
								temp=temp->link;
						}
						struct st *new=malloc(sizeof(struct st));
						new->data=n;
						new->link=NULL;
						temp->link=new;
				}
		}
		struct st *temp=*head;
		while(temp!=NULL)
		{
				printf("%d ",temp->data);
				temp=temp->link;
		}
		printf("\n");
		return;
}
void delete(struct st *head)
{
		struct st *p1=head;
		struct st *p2=head;
		struct st *p3=head;
		while(p1->link!=NULL)
		{
				p2=p1;
				p3=p2;
				//p2=p1->link;
				while(p2!=NULL)
				{
						//p3=p2;
						if(p1->data==p2->data)
						{
								//p3->link=p2->link;
								p3=p2;
								p2=p2->link;
								free(p3);
								p3=NULL;
								p1->link=p2;
						}
						else
						{
								//p3=p2;
								p2=p2->link;
						}
				}
				p1=p1->link;
		}
		struct st *t=head;
		while(t!=NULL)
		{
				printf("%d ",t->data);
				t=t->link;
		}
		printf("\n");
		return;
}

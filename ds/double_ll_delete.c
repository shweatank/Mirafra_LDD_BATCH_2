#include<stdio.h>
#include<stdlib.h>
struct dd
{
		int data;
		struct dd *prev;
		struct dd *next;
};
void add(struct dd **);
struct dd **delete(struct dd **, int);
int main()
{
		struct dd *head=malloc(sizeof(struct dd));
		add(&head);
		struct dd **t=delete(&head,2);
		while((*t)!=NULL)
		{
				printf("%d ",(*t)->data);
				*t=(*t)->next;
		}
		printf("\n");
		return 0;
}
void add(struct dd **head)
{
		struct dd *temp=*head;
		for(int i=0;i<5;i++)
		{
				struct dd *new=malloc(sizeof(struct dd));
				new->data=i+1;
				new->prev=NULL;
				new->next=NULL;
				if(*head==NULL)
				{
						*head=new;
				}
				else
				{
						while(temp->next!=NULL)
						{
								temp=temp->next;
						}
						new->prev=temp;
						temp->next=new;
				}
		}
}
struct dd **delete(struct dd **head, int n)
{
		struct dd *temp=*head;
		while(temp!=NULL)
		{
				if(temp->data==n)
				{
						if(temp->next==NULL)
						{
								(temp->prev)->next=temp->next;
								free(temp);
						}
						else
						{
					        	(temp->prev)->next=temp->next;
					        	(temp->next)->prev=temp->prev;
					        	free(temp);
						}
				}
				else
				{
						temp=temp->next;
				}
		}
		return head;
}

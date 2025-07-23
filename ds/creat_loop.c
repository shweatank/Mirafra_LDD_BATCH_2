#include<stdio.h>
#include<stdlib.h>
struct st
{
		int data;
		struct st *link;
};
int find_loop(struct st *head)
{
		struct st *temp1=head->link;
		struct st *temp2=head;
		while(temp1!=NULL && temp1->link!=NULL)
		{
				if(temp1==temp2)
				{
						return 1;
				}
				temp1=temp1->link->link;
				temp2=temp2->link;
		}
		return 0;
}
int main()
{
		int x=60;
		struct st *head=malloc(5*sizeof(struct st));
		for(int i=0;i<5;i++)
		{
				//head[i].data=i*2+10;
				head[i].link=&head[i+1];
		}
		head[4].link=NULL;
		head[0].data=10;
		head[1].data=20;
		head[2].data=30;
		head[3].data=40;
		head[4].data=50;
		struct st *temp=head;
		while(temp!=NULL)
		{
				printf("%d ",temp->data);
				temp=temp->link;
		}
		printf("\n");
		struct st *temp1=head;
		struct st *temp2=head;
		while(temp1!=NULL)                //creat loop
		{
				if(temp1->data==x)
				{
						while(temp2->link!=NULL)
						{
								temp2=temp2->link;
						}
						temp2->link=temp1;
						break;
				}
				else
				{
						temp1=temp1->link;
				}
		}
		int c=find_loop(head);
		if(c==1)
		{
				printf("loop found\n");
		}
		else
		{
				printf("loop not found\n");
		}
		return 0;
}

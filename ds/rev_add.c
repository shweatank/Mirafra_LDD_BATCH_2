#include<stdio.h>
#include<stdlib.h>
//void add(struct s **head);
//void rev(struct s **head);
struct s
{
		int data;
		struct s *link;
};
void add(struct s **);
void rev(struct s **);
void print(struct s **);
int main()
{
		struct s *head=malloc(sizeof(struct s));
		add(&head);
		print(&head);
		rev(&head);
		return 0;
}
void add(struct s **head)
{
		struct s *temp1=*head;
		for(int i=1;i<5;i++)
		{
				struct s *new=malloc(sizeof(struct s));
				new->data=i;
				new->link=NULL;
				if(temp1==NULL)
				{
						temp1=new;
				}
				else
				{
						new->link=temp1;
						temp1=new;
				}
				//temp=temp->link;
		}
		*head=temp1;
		//free(temp);
		//temp=NULL;
}
void print(struct s **head)
{
		struct s *temp2=*head;
		while(temp2!=NULL)
		{
				printf("%d ",temp2->data);
				temp2=temp2->link;
		}
		//free(temp);
		//temp=NULL;
		printf("\n");
		return;
}
void rev(struct s **head)
{
		struct s *c=*head;
		struct s *p=NULL;
		struct s *n=c;
		while(c!=NULL)
		{
				n=c->link;
				c->link=p;
				p=c;
				c=n;
		}
		*head=p;
		struct s *temp3=*head;
		while(temp3!=NULL)
		{
				printf("%d ",temp3->data);
				temp3=temp3->link;
		}
		//free(temp);
		//temp=NULL;
		printf("\n");
		return;
}


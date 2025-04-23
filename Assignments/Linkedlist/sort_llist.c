#include<stdio.h>
#include<stdlib.h>

struct node
{
	int data;
	struct node * next;
};
struct node* creat_node(int data);
void insert(int data);
void reverse();
void print();
struct node * head=NULL;
int main()
{

	insert(50);
	insert(20);
	insert(30);
	insert(5);
	insert(99);
	print();
	reverse();
	printf("\n");
	print();
}
struct node* creat_node(int data)
{
	struct node* New=(struct node*)malloc(sizeof(struct node*));
	if(New==NULL)
	{
		printf("Error");
		return NULL;
	}
	New->data=data;
	New->next=NULL;
	return New;
}
void insert(int data)
{
	struct node *New_node=creat_node(data);
	if(head==NULL)
	{
		head=New_node;
	}
	else
	{
		struct node* temp=head;
		while(temp->next!=NULL)
		{
			temp=temp->next;
		}
		temp->next=New_node;
	}
}    

void reverse()
{
	    if (head == NULL)
	    {
		    return;
	    }
	    struct node* temp=NULL;
	    struct node *temp2=NULL;
	    for(temp=head;temp!=NULL;temp=temp->next)
	    {
		    for(temp2=head;temp2!=NULL;temp2=temp2->next)
		    {
			    if(temp->data<temp2->data)
			    {
				    int ret=temp->data;
				    temp->data=temp2->data;
				    temp2->data=ret;
			    }
		    }
	    }

}
void print()
{
	struct node* temp=head;
	while(temp!=NULL)
	{
		printf("%d->",temp->data);
		temp=temp->next;
	}
	printf("\n");
}


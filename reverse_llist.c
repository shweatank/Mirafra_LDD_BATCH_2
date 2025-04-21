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

	insert(10);
	insert(20);
	insert(30);
	insert(40);
	insert(50);
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


        struct node *t1=NULL,*t2=head,*t3=head->next;
	while(t2->next!=NULL)
	{
		t2->next=t1;
		t1=t2;
		t2=t3;
		t3=t3->next;
       	}
	t2->next=t1;
	head=t2;

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


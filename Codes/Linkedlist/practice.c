#include <stdio.h>
#include <stdlib.h>

struct Node
{
	int data;
	struct Node* next;
};

int main()
{
	struct Node *head = NULL,*newNode = NULL,*temp = NULL;
	int i,n,count=0;
	printf("Enter No.of Nodes:");
	scanf("%d",&n);

	for(i=1;i<=n;i++)
	{
		newNode = (struct Node *)malloc(sizeof(struct Node));

		printf("Enter node data %d:",i);
		scanf("%d",&newNode->data);
		newNode->next = NULL;

		if(head == NULL)
		{
			head = newNode;
			temp = newNode;
		}
		else
		{
			temp->next = newNode;
			temp = newNode;
		}
	}

	printf("Linked List:");
	temp = head;
	while(temp!=NULL)
	{
		printf("%d->",temp->data);
		count++;
		temp = temp->next;
	}
	printf("NULL\n");
	printf("Count = %d\n",count);
}

#include"myheader.h"

STUDENT * DelLast(STUDENT *ptr)
{	STUDENT *temp;
	if(ptr==NULL)
	{
		printf("List is empty\n");
	}

	else if(ptr->link==NULL)  // list having only one node
	{
		free(ptr);
		ptr=NULL;

	}
	else
	{temp=ptr;
		while(temp->link->link)// cheching whether next node is Last node
		{
			temp=temp->link;
		}
		// after terminating the loop , temp is at last before node
		free(temp->link);
		temp->link=NULL;
	}
	return ptr;
}


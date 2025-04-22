#include"myheader.h"

int main()
{
	STUDENT *head=NULL;
	char choice;
	while(1)
	{
		printf("A.Add P.Print D.Del E.exit\n");
		printf("enter the choice\n");
		__fpurge(stdin);
		scanf("%c",&choice);
		switch(choice)
		{
			case 'A' : head=AddFirst(head);
				   break;
			case 'P' : Print(head);
				   break;
			case 'E' : exit(0);
			case 'D' : head=DelLast(head);
				   break;
			
		}


	}
}

/*
 ============================================================================
 Name        : queue.c
 Author      : shubham
 Version     :
 Copyright   : learning the code
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "queue.h"
int main(void) {
	int ele,choice;
	struct queue s1;
	s1.rear=-1;
	s1.front=-1;
	while(1)
	{
		printf("***************************\n");
		printf("1 for enqueue the data\n");
		printf("2 for dequeue the data\n");
		printf("3 for peep the data\n");
		printf("4 for display the data\n");
		printf("5 for exit\n");
		printf("****************************\n");

		printf("enter the choice\n");
		scanf("%d",&choice);

		switch(choice)
		{
		      case 1:printf("enter the element\n");
		             scanf("%d",&ele);
		             enqueue(&s1,ele);
		             break;
		      case 2://dequeue(&s1);
		             printf("delete data=%d\n",dequeue(&s1));
		             break;
		      case 3://peep(s1);
		             printf("current element=%d",peep(s1));
		             break;
		      case 4:display(s1);
		             break;
		      case 5:exit(0);
		             break;
		      default:printf("invalid result\n");
		              break;
		}
	}


	return EXIT_SUCCESS;
}

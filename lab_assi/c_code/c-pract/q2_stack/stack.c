/*
 ============================================================================
 Name        : stack.c
 Author      : shubham
 Version     :
 Copyright   : learning the code
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "stack.h"

int main(void)
{
	int choice,ele,item;
	struct stack s1;
	s1.top=-1;
	while(1)
	{
		printf("******************************\n");
		printf("Enter 1 for push operation \n");
		printf("Enter 2 for pop operation \n");
		printf("Enter 3 for peep operation \n");
		printf("Enter 4 for display operation \n");
		printf("Enter 5 for exit operation \n");
		printf("******************************\n");
		printf("Enter your choice \n");
		scanf("%d",&choice);

		switch(choice)
		{
		case 1:printf("enter the element\n");
		       scanf("%d",&ele);
		       push(&s1,ele);
		       break;
		case 2:item=pop(&s1);
		       printf("poped item=%d\n",item);
		       break;
		case 3:item=peep(s1);
		       printf("top item of stack=%d\n",item);
		       break;
		case 4:display(s1);
		       break;
		case 5:exit(0);
		default:printf("you entered wrong choice\n");


		}
	}

		return EXIT_SUCCESS;
}


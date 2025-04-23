/*
 * function.c
 *
 *  Created on: 05-May-2022
 *      Author: lenovo
 */

#include "stack.h"
void push(struct stack *s,int ele)
{
	if(s->top==SIZE-1)
	{
		printf("stack is full\n");
		return;
	}
	s->top++;
	s->arr[s->top]=ele;

}
int pop(struct stack *s)
{
	if(s->top==-1)
	{
		printf("stack is empty\n");
		return -99;
	}
	int item=s->arr[s->top];
	s->arr[s->top]=-99;
	s->top--;
	return item;
}

int peep(struct stack s)
{
	if(s.top==-1)
		{
			printf("stack is empty\n");
			return -99;
		}
	int item=s.arr[s.top];
	return item;
}
void display(struct stack s)
{
	if(s.top==-1)
			{
				printf("stack is empty\n");
				return;
			}
	while(s.top>-1)
	{
		printf("%d\t",s.arr[s.top]);
		s.top--;

	}
	printf("\n");
}


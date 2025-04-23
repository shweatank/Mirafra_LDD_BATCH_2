/*
 * stack.h
 *
 *  Created on: 05-May-2022
 *      Author: lenovo
 */


#ifndef STACK_HEADER_H_
#define STACK_HEADER_H_
#include<stdio.h>
#include<stdlib.h>
#define SIZE 5
struct stack
{
	int arr[SIZE];
	int top;
};
void push(struct stack *,int);
int pop(struct stack *);
int peep(struct stack);
void display (struct stack);

#endif /* STACK_HEADER_H_ */


/*
 * function.c
 *
 *  Created on: 05-May-2022
 *      Author: lenovo
 */

#include"queue.h"

void enqueue(struct queue *p,int ele)
{
	if(p->rear==SIZE-1)
	{
		printf("queue is full\n");
	}
	p->rear++;
	p->arr[p->rear]=ele;

	if(p->front==-1)
	{
		p->front=0;
	}
}
int dequeue(struct queue *p)
{
	if(p->front==-1)
	{
		printf("queue is empty\n");
		return -99;
	}
	int item=p->arr[p->front];
	p->arr[p->front]=-99;

	if(p->front==p->rear)
	{
		p->front=p->rear=-1;
	}
	else
	{
		p->front++;
	}
	return item;

}
int peep(struct queue p)
{
	if(p.front==-1)
	{
		printf("queue is empty\n");
	}
	if(p.rear==SIZE-1)
	{
		printf("queue is full\n");
	}
	int item=p.arr[p.front];
	return item;
}
void display(struct queue p)
{
	if(p.front==-1)
	{
		printf("queue is empty\n");
	}
	while(p.front<=p.rear)
	{
		printf("%d\t",p.arr[p.front]);
		p.front++;
	}
	printf("\n");
}



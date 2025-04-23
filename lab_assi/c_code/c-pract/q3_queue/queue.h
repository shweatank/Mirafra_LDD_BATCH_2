/*
 * queue.h
 *
 *  Created on: 05-May-2022
 *      Author: lenovo
 */

#ifndef QUEUE_HEADER_H_
#define QUEUE_HEADER_H_
#include<stdio.h>
#include<stdlib.h>
#define SIZE 5

struct queue
{
	int arr[SIZE];
	int rear;
	int front;
};

void enqueue(struct queue *p,int ele);
int dequeue(struct queue *p);
int peep(struct queue p);
void display(struct queue p);

#endif /* QUEUE_HEADER_H_ */

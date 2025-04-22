#include<stdio.h>
#include<stdlib.h>

typedef struct Student
{
	int roll;
	char name[20];
	struct Student *link;
}STUDENT;

STUDENT * AddFirst(STUDENT *);
STUDENT * DelLast(STUDENT *);
void Print(STUDENT *);


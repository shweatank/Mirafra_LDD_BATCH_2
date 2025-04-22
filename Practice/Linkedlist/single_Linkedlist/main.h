#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAIN_H
#define MAIN_H

#define success 0
#define failure -1

typedef struct node{
	int data;
	struct node *link;
}slist;


int insert_at_first(slist **head, int Ndata);
int insert_at_last(slist **head, int Ndata);
int printlist(slist *head);
int Delete_at_first(slist **head);
int Delete_at_last(slist **head);
int Delete_list(slist *head);

#endif

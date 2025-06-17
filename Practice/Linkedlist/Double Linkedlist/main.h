#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct node {
    struct node *next;
    struct node *prev;
    int data;
} dlist;

#define success 0
#define failure -1


int insert_at_last(dlist **head, dlist **tail, int ndata) ;
int print_list(dlist *head) ;
int delete_node(dlist **head, dlist **tail, int ndata);
int delete_last(dlist **head, dlist **tail);
int reverse_double_linkedlist(dlist **head);

#endif

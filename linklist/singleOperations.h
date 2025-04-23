#include<stdlib.h>

enum {
  PASS = 0 , FAIL = 1
};
typedef struct node{

	int data;
	struct node * link;
}node;

node * create_node(int);
int insert_atend(node **, int);
int insert_atbegin(node **, int);

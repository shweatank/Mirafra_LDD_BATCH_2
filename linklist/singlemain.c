#include<stdio.h>
#include"singleOperations.h"


int main(){

	node* head =NULL;
	node * ptr = NULL;
	int NoOfNodes = 0,value = 0;
	printf("enter the Noof nodes -  bwgin");
        scanf("%d",&NoOfNodes);

       for(int i = 0; i < NoOfNodes; i++){
                value = rand();
	//	printf("next node value   ");
	//	scanf("%d",&value);
		if(insert_atbegin(&head,value) == PASS){
			continue;
		}else {
		 printf("failed to get");
		}
	}
      
	ptr =  head;
	while(ptr != NULL){
		printf("%d->",ptr->data);
		ptr = ptr->link;
	}

	free(head);
	return 0;

}

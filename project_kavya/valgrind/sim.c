#include<stdio.h>
#include<stdlib.h>
int main()
{
		void *ptr=malloc(34*sizeof(char));
		free(ptr);
		free(ptr);
		return 0;
}

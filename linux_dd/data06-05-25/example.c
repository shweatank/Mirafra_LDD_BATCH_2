#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct strc{

int a;
char b;
char c[10];};

char buffer_t[100];
int main()
{
	struct strc s ={10,'s',"askash"};
	struct strc * buffer =(struct strc* )malloc(sizeof(struct strc));
	memcpy(buffer,&s,sizeof(struct strc));
	// Copy the struct into the char buffer
	    memcpy(buffer_t, &s, sizeof(struct strc));

	    // To interpret the buffer as a struct again:
	    struct strc* recovered = (struct strc*)buffer_t;

	    // Accessing the recovered struct
	    printf("%d %c %s\n", recovered->a, recovered->b, recovered->c);

	
	printf("%d %c %s",buffer->a,buffer->b,buffer->c);
	
return 0;
}

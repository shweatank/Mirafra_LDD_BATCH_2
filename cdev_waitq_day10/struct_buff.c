#include<stdio.h>
#include<stdlib.h>
#include<string.h>
struct st
{
	int a;
	char b;
	char name[20];
};
int main()
{
	struct st s,s1;
	s.a=143;
	s.b='v';
	strcpy(s.name,"welcome to mirafra");
	struct st * p=malloc(1*sizeof(s));
	memmove(p,(struct st *)&s,sizeof(s));
	s1=*p;
	printf("%d %c %s\n",s1.a,s1.b,s1.name);



}


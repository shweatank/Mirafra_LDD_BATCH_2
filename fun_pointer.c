#if 0                               //Event dispatcher
#include<stdio.h>
#include "practice.h"
void add(int a,int b)
{
		printf("add: %d\n",a+b);
}
void sub(int a,int b)
{
		printf("sub: %d\n",a-b);
}
void mul(int a,int b)
{
		printf("mul: %d\n",a*b);
}
void div(int a,int b)
{
		if(b!=0)
		{
				printf("div : %d\n",a/b);
		}
		else
		{
				printf("division not possible\n");
		}
}
int main()
{
		int n1,n2,ch;
		printf("Enter numbers :\n");
		scanf("%d %d",&n1,&n2);
		printf("Enter choice:\n");
		scanf("%d",&ch);
        enum fun var=ch;
		void (*fp[max])(int,int)={add,sub,mul,div};
		fp[var](n1,n2);
		return 0;
}
#endif

#if 0              //Math expression evaluator
#include<stdio.h>
void add(int a,int b)
{
printf("add: %d\n",a+b);
}
void sub(int a,int b)
{
printf("sub: %d\n",a-b);
}
void mul(int a,int b)
{
		printf("mul: %d\n",a*b);
}
void div(int a,int b)
{
		if(b!=0)
		{
				printf("div: %d\n",a/b);
		}
		else
		{
				printf("division not possible\n");
		}
}
int main()
{
int n1,n2;
char ch;
printf("Enter expression\n");
scanf("%d %c %d",&n1,&ch,&n2);
void (*fp[4])(int,int)={add,sub,mul,div};
if(ch=='+')
{
		fp[0](n1,n2);
}
if(ch == '-')
{
		fp[1](n1,n2);
}
if(ch == '*')
{
		fp[2](n1,n2);
}
if(ch == '/')
{
		fp[3](n1,n2);
}
}
#endif

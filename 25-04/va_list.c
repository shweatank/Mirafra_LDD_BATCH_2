#include<stdio.h>
#include<stdarg.h>
int sum(int count,...)
{
	int s=0;
	va_list arg;
	va_start(arg,count);
	for(int i=0;i<count;i++)
	{
		int value=va_arg(arg,int);
		s+=value;
	}
	return s;
}
int main()
{
	int r=sum(4,1,2,3,5);
	printf("%d\n",r);
}

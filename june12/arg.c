#include<stdio.h>
#include<stdarg.h>
int sum(int count,...)
{
	int s=0;
	va_list(arg);
	va_start(arg,count);
	for(int i=0;i<count;i++)
	{
		int value=va_arg(arg,int);
		s += value;
	}
	va_end(arg);
	return s;
}

int main(){
int r=sum(3,1,2,3);
printf("%d\n",r);
}

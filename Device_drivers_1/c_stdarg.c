#include <stdio.h>
#include <stdarg.h>

int sum_of_numbers(int count, ...)
{
	int sum = 0;

	va_list args;
	va_start(args,count);

	for(int i=0;i<count;i++)
	{
		int value = va_args(args,int);
		sum += value;
	}

	va_end(args);
	return sum;
}

int main()
{
	
}

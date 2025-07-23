#if 0
#include<stdio.h>

//void (*fptr)()=fun;

static void fun()
{
		printf("hello world\n");
		return;
}
void (*fptr)()=fun;
#endif

#if 1
int *ptr;
void fun()
{
		static int b=10;
		ptr=&b;
}
#endif

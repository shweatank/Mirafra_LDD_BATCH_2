#if 0
#include<stdio.h>
int main()
{
		extern void (*fptr)();
		fptr();
		return 0;
}
#endif

#if 1
#include<stdio.h>
void fun();
int main()
{
		fun();
		extern int *ptr;
		printf("%d\n",*ptr);
		return 0;
}
#endif

#include<stdio.h>
#if 0
#line 200 "file"   //used to set line numbers

int main()
{
		printf("this is line from %s at line number %d\n",__FILE__, __LINE__);
		return 0;
}
#endif

#if 1
#define S(m,n) m##n
int main()
{
		printf("%d\n",S(20,30));
		return 0;
}
#endif

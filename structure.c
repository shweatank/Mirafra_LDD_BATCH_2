#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#if 0
struct st
{
		int p;
		char *ptr;
};
int main()
{
		struct st *s;
		s=malloc(sizeof(struct st));
		s->p=10;
		s->ptr="kavya";
		printf("%d\n%s\n",s->p,s->ptr);
		return 0;
}
#endif
#if 0
struct st
{
		int p;
		char ch[20];
};
int main()
{
		struct st s;
		s.p=10;
		//s.ch="kavya";
		strcpy(s.ch,"kavya");
		printf("%d\n%s\n",s.p,s.ch);
		return 0;
}
#endif
#if 1
struct st
{
		int id;
};
int main()
{
		struct st s[5];
		printf("Enter id numbers:\n");
		for(int i=0;i<5;i++)
		{
				scanf("%d",&s[i].id);
				printf("%d ",s[i].id);
		}
		return 0;
}
#endif

#include<stdio.h>
#if 0
struct st
{
		int a,b;
		void (*fp)(int,int);
};
void add(int a,int b)
{
		printf("add: %d\n",a+b);
}
void mul(int a,int b)
{
		printf("mul: %d\n",a*b);
}
int main()
{
		struct st s={10,20,add};
		struct st *p=&s;
		//p->fp(p->a,p->b);
		p->a=4;
		p->b=5;
		p->fp=mul;
		p->fp(p->a,p->b);
		return 0;
}
#endif

#if 0
#include<string.h>
struct st
{
		int a;
		char ch[20];
		//char *ch;
};
int main()
{
		struct st s;
		struct st *p=&s;
		p->a=10;
		//p->ch="kavya";           //only when char *ch
		strcpy(p->ch,"kavya");     //if we use char ch[20];
		printf("%d\n%s\n",p->a,p->ch);
		return 0;
}
#endif

#if 0                //callback function
void disp(void (*fp)(int,int))
{
		fp(4,5);
}
void sum(int a,int b)
{
		printf("%d\n",a+b);
}
int main()
{
		disp(sum);
		return 0;
}
#endif

#if 0                             //pass by value
struct st
{
		int a;
};
void disp(struct st k)
{
		k.a=10;
		printf("%d\n",k.a);
}
int main()
{
		struct st s;
		disp(s);
		return 0;
}
#endif

#if 1                       //pass by reference
struct st
{
		int a;
};
void disp(struct st *p)
{
		p->a=10;
		printf("%d\n",p->a);
		(*p).a=20;
		printf("%d\n",p->a);
}
int main()
{
		struct st s;
		disp(&s);
		return 0;
}
#endif

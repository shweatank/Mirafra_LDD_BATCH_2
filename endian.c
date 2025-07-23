#include<stdio.h>
#if 0
int main()
{
		int x=12345678;
		char *ptr=&x;
		if(*ptr==78)
		{
				printf("little endian\n");
		}
		else
		{
				printf("big endian\n");
		}
		return 0;
}
#endif

#if 0
union u
{
		int n;
		char byte[4];
};
int main()
{
		union u u1;
		u1.n=0x12345678;
		printf("before: %x\n",u1.n);
		int temp;
		temp=u1.byte[0];
		u1.byte[0]=u1.byte[3];
		u1.byte[3]=temp;

		temp=u1.byte[1];
		u1.byte[1]=u1.byte[2];
		u1.byte[2]=temp;
		printf("after: %x\n",u1.n);
		return 0;
}
#endif

#if 1
int main()
{
		char ch;
		printf("Enter character\n");
		scanf("%hhx",&ch);
		char c=((ch & 0x0F)<<4) | ((ch & 0xF0)>>4);
		printf("nibble: %hhx\n",c);
		return 0;
}
#endif

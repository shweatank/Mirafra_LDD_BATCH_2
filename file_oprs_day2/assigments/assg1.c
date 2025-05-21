#include<stdio.h>
#include<stdlib.h>
typedef enum{add=0,sub,mul}obj;

typedef int (*handler)(int ,int);
int ADD(int x,int y)
{
	return x+y;
}
int SUB(int x,int y)
{
        return x-y;
}
int MUL(int x,int y)
{
        return x*y;
}

handler arr[]={ADD,SUB,MUL};

int funcs_handler(handler *p,obj v,int a,int b)
{int res;
	res=p[v](a,b);
	printf("result:%d\n",res);
}
int main()
{
int op1,op2,choice;
while(1)
{
	printf("______enter the choice______\n 1.ADD 2.SUB 3.MUL 4.EXIT\n");
	scanf("%d",&choice);
	if(choice==4)
	{
		exit(0);
	}
	printf("enter the op1 and op2 values:\n");
	scanf("%d%d",&op1,&op2);
	switch(choice)
	{
		case 1:funcs_handler(arr,add,op1,op2);
		       break;
		case 2:funcs_handler(arr,sub,op1,op2);
                       break;
	        case 3:funcs_handler(arr,mul,op1,op2);
                       break;


	}
}
}


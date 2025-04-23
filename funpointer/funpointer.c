#include<stdio.h>

int add(int a, int b){return a+b;}
int sub(int a, int b){return a-b;}
int mul(int a, int b){return a*b;}

int main(){

	int(*operations[3])(int,int) ={};
	operations[0] = add;
	operations[1] =  sub;
	operations[2] = mul;
	printf("sum = %d\n",operations[0](2,4));
	printf("sub  = %d\n",operations[1](2,4));
	printf("mul  = %d\n",operations[2](2,4));

	return 0;
}

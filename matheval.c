/*
Math Expression Evaluator
Problem Statement:
Create a simple expression evaluator where the user inputs an expression like "5 * 2" or "10 + 3", and the evaluator parses the operator and calls the right function using a function pointer table.

Skills Used: Function pointer arrays, parsing logic, input validation.
*/

#include <stdio.h>

int add(int a,int b)
{
	return a+b;
}


int sub(int a,int b)
{
	return a-b;
}

int mul(int a,int b)
{
	return a*b;
}


int div(int a,int b)
{
	if( b == 0)
	{
		return 0;
	}
	return a/b;
}


int main()
{
	int a, b ;
	char c;
	scanf("%d %c %d",&a,&c,&b);
	//printf("%d-%d-%c",a,b,c);
	if((c!='+')&&(c!='-')&&(c!='*')&&(c!='/'))
	{
		printf("invalid ");
	}
	
	int (*ptr[4])(int ,int ) ={&add,&sub,&mul,&div};
	int val;
	switch(c){
	case '+':
		val = ptr[0](a,b);
		printf("%d",val);
		break;
	
	case '-' :
		val = ptr[1](a,b);
		printf("%d",val);
		break;
	
	case '*' :
		val = ptr[2](a,b);
		printf("%d",val);
		break;
	case '/':
		val = ptr[3](a,b);
		printf("%d",val);
		break;
		
	default : break;
	
	}
	return 0;
}

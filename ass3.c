#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
int add(int a,int b)
{
return (a+b);
}
int sub(int a,int b)
{
return (a-b);
}
int mul(int a,int b)
{
return (a*b);
}
int divi(int a,int b)
{
return (a/b);
}
int main()
{
int (*fun[4])(int ,int)={&add,&sub,&mul,&divi};
  char str[10],num1[10],num2[10];
  printf("Enter the string:");
  scanf("%[^\n]",str);
  int i=0,j=0;
  while(isdigit(str[i]))
  {
  num1[j++]=str[i++];
  }
  num1[j]='\0';
  char op=str[i++];
  j=0;
  while(isdigit(str[i]))
  {
  num2[j++]=str[i++];
  }
  num2[j]='\0';
  int a=atoi(num1);
  int b=atoi(num2);
  switch(op)
  {
  case '+':
  {
         printf("%d\n",fun[0](a,b));
         break;
   }
   case '-':
  {
         printf("%d\n",fun[1](a,b));
         break;
   }
   case '*':
  {
         printf("%d\n",fun[2](a,b));
         break;
   }
   case '/':
  {
         printf("%d\n",fun[3](a,b));
         break;
   }
   }
   }
           
  

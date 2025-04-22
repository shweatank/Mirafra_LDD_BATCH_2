#include <stdio.h>

void add(int a,int b,int opt)
{
  
  if(opt == 0)
  {
      printf("Sum = %d\n",a+b);
  }
}

void sub(int a,int b,int opt)
{
  
  if(opt ==1)
  {
    printf("Sub = %d\n",a-b);
  }

}

void Mul(int a,int b,int opt)
{
  
  if(opt == 2)
  {
    printf("Mul = %d\n",a*b);
  }
}

void Div(int a,int b,int opt)
{
  
  if(opt == 3)
  {
    printf("Div = %d\n",a/b);
  }
}

int main()
{
  int num1,num2,choice;
  printf("Enter a, b values:");
  scanf("%d %d",&num1,&num2);

  printf("Enter choice :");
  scanf("%d",&choice);

  void (*fp[4])(int,int,int) = {add,sub,Mul,Div};

  if(choice<=3 || choice>=0)
  {
    fp[choice](num1,num2,choice);

  }










}

#include<stdio.h>
int operator(char op);
int add(int num1,int num2){ return num1+num2;}
int sub(int num1,int num2){ return num1-num2;}
int mul(int num1,int num2){return num1*num2;}
int div(int num1,int num2){return num2!=0? num1/num2:0;}
int main()
{
    int num1,num2;
    char op;
    printf("Enter the string (e.g 5*2 ) ");
    scanf("%d %c %d", &num1, &op, &num2);
    int (*operation[4])(int ,int ) = {add,sub,mul,div}; //fn pointer array
    int ret = operator(op);
    if(ret==-1)
    {
        printf("Default operator : ");
        return 1;
    }
    int result = operation[ret](num1,num2);
    printf("The result is %d" ,result);
    return 0;
}

int operator(char op)
{
    switch(op)
    {
        case '+':
        {
            return 0;
        }
        case '-':
        {
            return 1;
            
        }
        case '*':
        {
            return 2;
        }
        case '/':
        {
            return 3;
        }
        default:
        {
            return -1;
        }
    }
}

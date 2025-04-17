/*Math Expression Evaluator
Problem Statement:
Create a simple expression evaluator where the user inputs an expression like "5 * 2" or "10 + 3", and the evaluator parses the operator and calls the right function using a function pointer table.

Skills Used: Function pointer arrays, parsing logic, input validation.*/


#include <stdio.h>

float add(int n1, int n2)
{
    return n1+n2;
}
float sub(int n1, int n2)
{
    return n1 - n2;
}
float mul(int n1, int n2)
{
    return n1 * n2;
}
float division(int n1, int n2)
{
    return n1 / n2;
}
int main()
{
    char str[50];
    printf("Enter the Expression string : \n");
    fgets(str, sizeof(str), stdin);
    
    //parse or split the string
    int num1, num2;
    char op;
    if(sscanf(str, "%d %c %d", &num1, &op, &num2) > 0)
    {
        printf("num1 is  =  %d\n", num1);
        printf("opeartor is  =  %c\n", op);
        printf("num2 is  =  %d\n", num2);
    }
    else
    {
        printf("Invalid Expression");
        
    }
    
    float (*fptr)(int, int);
    float res;
    switch(op)
    {
        case '+':
        fptr = add;
        res = (* fptr)(num1, num2);
        printf("Results %.2f", res);  
        break;
        
        case '-':
        fptr = sub;
        res = (* fptr)(num1, num2);
        printf("Results %.2f", res);  
        break;
        
        case '*':
        fptr = mul;
        res = (* fptr)(num1, num2);
        printf("Results %.2f", res);  
        break;
        
        case '/':
        fptr  = division;
        res = (* fptr)(num1, num2);
        printf("Results %.2f", res);  
        break;
        
        default :
        printf("INvalid");
        break;
    }
     
}

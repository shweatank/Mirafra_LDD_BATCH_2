/*Math Expression Evaluator
Problem Statement:
Create a simple expression evaluator where the user inputs an expression like "5 * 2" or "10 + 3", and the evaluator parses the operator and calls the right function using a function pointer table.

Skills Used: Function pointer arrays, parsing logic, input validation.*/


#include <stdio.h>

// Define arithmetic functions
int add(int a, int b)     
{
  return a + b; 
}
int subtract(int a, int b)
{ 
  return a - b; 
}
int multiply(int a, int b)
{
  return a * b; 
}
int divide(int a, int b)  
{ 
    if (b == 0) 
    {
        printf("Error: Division by zero!\n");
        return 0;
    }
    return a / b; 
}

int main() {
    int num1, num2, result;
    char operator;


    int (*operations[4])(int, int) = { add, subtract, multiply, divide };
    char operators[4] = { '+', '-', '*', '/' };

    printf("Enter expression (e.g., 5 * 2): ");
    scanf("%d %c %d", &num1, &operator, &num2);

    int i;
    for (i = 0; i < 4; i++) 
    {
        if (operator == operators[i]) 
        {
            result = operations[i](num1, num2);
            printf("Result: %d\n", result);
            return 0;
        }
    }

    printf("Error: Unsupported operator!\n");
    return 1;
}

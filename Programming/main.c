#include <stdio.h>
#include "operations.h"

int main() {
    float num1, num2;
    char op;
    float (*operation)(float, float);
    
    printf("Enter operation: ");
    scanf("%f %c %f", &num1, &op, &num2);
    
    switch(op) {
        case '+':
            operation = add;
            break;
        case '-':
            operation = sub;
            break;
        case '*':
            operation = mul;
            break;
        case '/':
            operation = divide;
            break;
        default:
            printf("Invalid operation\n");
            return 1;
    }
    
    float result = operation(num1, num2);
    printf("Result: %.2f\n", result);
    
    return 0;
}


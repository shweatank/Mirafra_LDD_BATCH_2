#include <stdio.h>


int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);

int main() {
    int (*operation)(int, int); //fn ptr
    
    int num1, num2;
    char op;


    printf("Enter two integers: ");
    scanf("%d %d", &num1, &num2);

    printf("Enter operation (+, -, *, /): ");
    scanf(" %c", &op);  
    switch(op) {
        case '+':
            operation = add;
            break;
        case '-':
            operation = subtract;
            break;
        case '*':
            operation = multiply;
            break;
        case '/':
            operation = divide;
            break;
        default:
            printf("Invalid operator!\n");
            return 1;
    }

 
    int result = operation(num1, num2); //call the fn using fn pointer
    printf("Result: %d\n", result);

    return 0;
}

// Function definitions
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

int divide(int a, int b) {
    if (b != 0) {
        return a / b;
    } else {
        printf("Error: Division by zero!\n");
        return 0;  
    }
}


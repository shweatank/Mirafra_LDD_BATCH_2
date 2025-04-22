#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef double (*operation_fn)(double, double);


double add(double a, double b) { return a + b; }
double subtract(double a, double b) { return a - b; }
double multiply(double a, double b) { return a * b; }
double divide(double a, double b) {
    if (b == 0) {
        fprintf(stderr, "Error Division by zero\n");
        exit(EXIT_FAILURE);
    }
    return a / b;
}

int get_operator_index(char op) {
    switch (op) {
        case '+': return 0;
        case '-': return 1;
        case '*': return 2;
        case '/': return 3;
        default: return -1;
    }
}

int main() {
char input[100];
double a, b;
char op;

operation_fn operations[] = { add, subtract, multiply, divide };

printf("Enter a math expression: ");
if (!fgets(input, sizeof(input), stdin)) {
   fprintf(stderr, "Error reading input.\n");
   return EXIT_FAILURE;
}

if (sscanf(input, "%lf %c %lf", &a, &op, &b) != 3) {
   fprintf(stderr, "Invalid expression format\n");
   return EXIT_FAILURE;
}

int index = get_operator_index(op);
if (index == -1) {
   fprintf(stderr, "Unsupported operator: %c\n", op);
   return EXIT_FAILURE;
}

double result = operations[index](a, b);
printf("Result: %.2f\n", result);

return 0;
}


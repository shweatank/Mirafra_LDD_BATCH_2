#include <stdio.h>
#include <stdarg.h>

// Function to calculate the sum of 'count' integers
int sum_of_numbers(int count, ...)
{
    int sum = 0;

    va_list args;              // Declare variable argument list
    va_start(args, count);     // Initialize args to store values after 'count'

    for (int i = 0; i < count; i++) {
        int value = va_arg(args, int);  // Fetch next integer
        sum += value;
    }

    va_end(args);              // Clean up

    return sum;
}

int main()
{
    int result = sum_of_numbers(7, 10, 20, 30, 40, 50, 60, 70);  // 4 arguments
    printf("Sum: %d\n", result);

    return 0;
}

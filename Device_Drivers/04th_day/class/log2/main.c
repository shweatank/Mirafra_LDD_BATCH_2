#include <stdio.h>
#include <stdarg.h>  // Required for va_list, va_start, va_arg, va_end

// Function that takes variable number of arguments
void my_logger(const char *format, ...)
{
    va_list args;  // To hold the variable argument list

    va_start(args, format); // Initialize args to store all values after 'format'

    // Loop through the format string
    while (*format != '\0') {
        if (*format == 'd') {
            int i = va_arg(args, int);  // Fetch next int
            printf("int: %d\n", i);
        } else if (*format == 'c') {
            char c = (char)va_arg(args, int); // char is promoted to int
            printf("char: %c\n", c);
        } else if (*format == 'f') {
            double f = va_arg(args, double);  // float is promoted to double
            printf("float: %.2f\n", f);
        } else if (*format == 's') {
            char *s = va_arg(args, char *);  // Fetch string
            printf("string: %s\n", s);
        }
        format++;
    }

    va_end(args); // Clean up the va_list
}

int main()
{
    // Example usage
    my_logger("dcsf", 10, 'A', "Hello", 3.14);

    return 0;
}

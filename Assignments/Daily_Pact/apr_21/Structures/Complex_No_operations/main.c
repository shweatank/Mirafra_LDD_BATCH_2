#include <stdio.h>

struct Complex {
    float real;
    float imag;
};

void printComplex(struct Complex c) {
    printf("Complex Number: %.2f + %.2fi\n", c.real, c.imag);
}

struct Complex addComplex(struct Complex c1, struct Complex c2) {
    struct Complex result;
    result.real = c1.real + c2.real;
    result.imag = c1.imag + c2.imag;
    return result;
}

int main() {
    struct Complex c1 = {2.0, 3.0};
    struct Complex c2 = {1.5, 4.5};
    struct Complex result = addComplex(c1, c2);
    printComplex(result);
    return 0;
}


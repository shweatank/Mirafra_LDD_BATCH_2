#include <stdio.h>
#include "operations.h"

float add(float a, float b) {
    return a + b;
}

float sub(float a, float b) {
    return a - b;
}

float mul(float a, float b) {
    return a * b;
}

float divide(float a, float b) {
    if (b == 0) {
        printf("Error Division by zero\n");
        return 0;
    }
    return a / b;
}
	

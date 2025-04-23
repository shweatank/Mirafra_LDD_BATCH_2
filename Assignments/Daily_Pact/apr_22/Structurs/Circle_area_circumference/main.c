#include <stdio.h>
#define PI 3.14159

struct Circle {
    float radius;
};

float area(struct Circle c) {
    return PI * c.radius * c.radius;
}

float circumference(struct Circle c) {
    return 2 * PI * c.radius;
}

int main() {
    struct Circle c = {7.0};
    printf("area: %.2f\n", area(c));
    printf("circumference: %.2f\n", circumference(c));
    return 0;
}


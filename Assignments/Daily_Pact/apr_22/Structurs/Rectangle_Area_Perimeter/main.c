#include <stdio.h>

struct Rectangle {
    float length;
    float width;
};

float area(struct Rectangle r) {
    return r.length * r.width;
}

float perimeter(struct Rectangle r) {
    return 2 * (r.length + r.width);
}

int main() {
    struct Rectangle rect = {5.0, 3.0};
    printf("area: %.2f\n", area(rect));
    printf("perimeter: %.2f\n", perimeter(rect));
    return 0;
}


#include <stdio.h>
#include <math.h>

struct Point3D {
    float x;
    float y;
    float z;
};

float distance(struct Point3D p1, struct Point3D p2) {
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + 
                (p2.y - p1.y) * (p2.y - p1.y) + 
                (p2.z - p1.z) * (p2.z - p1.z));
}

int main() {
    struct Point3D p1 = {1.0, 2.0, 3.0};
    struct Point3D p2 = {4.0, 5.0, 6.0};
    printf("distance: %.2f\n", distance(p1, p2));
    return 0;
}


#include <stdio.h>
#include <math.h>

#define PI 3.14159


float area_circle() 
{
    float radius;
    printf("Enter radius: ");
    scanf("%f", &radius);
    return PI * radius * radius;
}


float area_rectangle() 
{
    float length, width;
    printf("Enter length and width: ");
    scanf("%f %f", &length, &width);
    return length * width;
}

float area_triangle() 
{
    float base, height;
    printf("Enter base and height: ");
    scanf("%f %f", &base, &height);
    return 0.5 * base * height;
}

int main()
{
    int choice;
    float (*area_func[3])() = {area_circle, area_rectangle, area_triangle};

    printf("Choose a shape to calculate area:\n");
    printf("0. Circle\n");
    printf("1. Rectangle\n");
    printf("2. Triangle\n");
    printf("Enter your choice (0-2): ");
    scanf("%d", &choice);

    if (choice >= 0 && choice <= 2) 
    {
        float result = area_func[choice]();
        printf("Area = %.2f\n", result);
    }
    else
    {
        printf("Invalid choice!\n");
    }

    return 0;
}

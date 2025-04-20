#include <stdio.h>
#include <string.h>

struct Student {
    char name[50];
    int rollNo;
    float marks;
};

int main() {
    struct Student s;

    printf("what is the students name ");
    fgets(s.name, 50, stdin);
    s.name[strcspn(s.name, "\n")] = 0;

    printf("roll no ");
    scanf("%d", &s.rollNo);

    printf("marks ");
    scanf("%f", &s.marks);

    printf("\nhere is the info:\n");
    printf("Name: %s\n", s.name);
    printf("roll No: %d\n", s.rollNo);
    printf("marks: %.2f\n", s.marks);
    return 0;
}

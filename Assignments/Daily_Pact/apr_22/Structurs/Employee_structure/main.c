#include <stdio.h>

struct Employee {
    int empID;
    char name[50];
    float salary;
};

void printEmployee(struct Employee e) {
    printf("Employee ID: %d\n", e.empID);
    printf("Name: %s\n", e.name);
    printf("Salary: %.2f\n", e.salary);
}

int main() {
    struct Employee emp = {1001, "akash", 55000.0};
    printEmployee(emp);
    return 0;
}


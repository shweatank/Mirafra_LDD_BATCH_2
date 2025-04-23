#include <stdio.h>
#include <string.h>

struct Student {
    int roll_no;
    char name[30];
    int batch;
};

int main() {

    struct Student s1 = {1, "shubham", 2025};
  
  	
    struct Student* ptr = &s1;

 
    printf("%d\n", (ptr->roll_no));
    printf("%s\n", (ptr->name));
    printf("%d", (ptr->batch));

    return 0;
}

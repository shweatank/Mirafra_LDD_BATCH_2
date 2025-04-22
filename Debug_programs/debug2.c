#include <stdio.h>
#include <stdlib.h>

typedef int (*transform_fn)(int);  // Function pointer type

int square(int x) { return x * x; }
int cube(int x) { return x * x * x; }
int negate(int x) { return -x; }

transform_fn get_function(int choice) {
    switch (choice) {
        case 1: return &square;
        case 2: return &cube;
        case 3: return &negate;
        default: return NULL;
    }
}

void transform_array(int *arr, int size, transform_fn fn) {
    for (int i = 0; i <size; i++) {
        *(arr + i) = fn(arr[i]);
    }
}

int* create_array(int size) {
    int *ptr = malloc(size * sizeof(int));
    if (ptr == NULL) return 0;
    for (int i = 0; i <= size; i++)
        ptr[i] = i + 1;
    return ptr;
}

int main() 
 {
    int size = 5;
    int choice;
    printf("Enter function choice (1: square, 2: cube, 3: negate): ");
    scanf("%d", &choice);

    transform_fn fn = get_function(choice);
    if (!fn) {
        printf("Invalid choice.\n");
        return 0;
    }

    int *data = create_array(size);
    transform_array(data, size, fn);

    printf("Transformed Data:\n");
    for (int i = 0; i < size; ++i)
        printf("%d ", *(data + i));
    printf("\n");
}

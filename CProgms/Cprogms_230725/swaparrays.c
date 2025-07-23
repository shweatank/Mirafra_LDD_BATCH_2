#include <stdio.h>

void swapArrays(int *arr1, int size1, int *arr2, int size2) {
    int min = size1 < size2 ? size1 : size2;

    for (int i = 0; i < min; i++) {
        int temp = arr1[i];
        arr1[i] = arr2[i];
        arr2[i] = temp;
    }

    if (size1 != size2) {
        printf(" Arrays have different sizes. Only first %d elements were swapped.\n", min);
    }
}

void printArray(const char *label, int *arr, int size) {
    printf("%s: ", label);
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int array1[] = {1, 2, 3, 4, 5};
    int array2[] = {10, 20, 30};

    int size1 = sizeof(array1) / sizeof(array1[0]);
    int size2 = sizeof(array2) / sizeof(array2[0]);

    printf("Before swap:\n");
    printArray("Array 1", array1, size1);
    printArray("Array 2", array2, size2);

    swapArrays(array1, size1, array2, size2);

    printf("\nAfter swap:\n");
    printArray("Array 1", array1, size1);
    printArray("Array 2", array2, size2);

    return 0;
}


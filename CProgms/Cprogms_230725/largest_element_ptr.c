#include <stdio.h>

int findLargest(int *arr, int size) {
    int *ptr = arr;
    int max = *ptr;

    for (int i = 1; i < size; i++) {
        ptr++;
        if (*ptr > max)
            max = *ptr;
    }

    return max;
}

int main() {
    int array[] = {10, 35, 2, 98, 45, 23, 67};
    int size = sizeof(array) / sizeof(array[0]);
    printf("\n");
    for(int i = 0; i < size ; i++) 
	   printf("%d  ", array[i]); 
    int largest = findLargest(array, size);

    printf("\nThe largest element is: %d\n", largest);

    return 0;
}


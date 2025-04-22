#include <stdio.h>
#include <stdlib.h>


int is_even(int n) {
    return n % 2 == 0;
}


int is_positive(int n) {
    return n > 0;
}


void filter(int *arr, int size, int (*condition)(int)) 
{
    for (int i = 0; i < size; ++i) 
    {
        if (condition(arr[i])) 
        {
            printf("%d ", arr[i]);
        }
    }
}

int main() {
    int arr[] = {5, -3, 2, 8, -1, 10, 7};
    int size = sizeof(arr) / sizeof(arr[0]);

    
    int (*f[2])(int) = {is_positive, is_even};

    
    printf("Positive numbers: ");
    filter(arr, size, f[0]);
    printf("\n");

    printf("Even numbers: ");
    filter(arr, size, f[1]);
    printf("\n");

    return 0;
}


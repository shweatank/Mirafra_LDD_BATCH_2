#include <stdio.h>


typedef int (*Comparator)(int, int);


void swap(int *a, int *b) {
    int temp=*a;
    *a=*b;
    *b=temp;
}

int partition(int arr[], int low, int high, Comparator cmp) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (cmp(arr[j], pivot) < 0) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quickSort(int arr[], int low, int high, Comparator cmp) {
    if (low < high) {
        int pi = partition(arr, low, high, cmp);

        quickSort(arr, low, pi - 1, cmp);
        quickSort(arr, pi + 1, high, cmp);
    }
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int ascending(int a, int b) {
    return a - b;
}

int descending(int a, int b) {
    return b - a;
}

int evenFirst(int a, int b) {
    if (a % 2 == 0 && b % 2 != 0) return -1;
    if (a % 2 != 0 && b % 2 == 0) return 1;
    return a - b; 
}

int main() {
    int arr[] = {9, 3, 4, 2, 8, 5, 1, 6};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Original: ");
    printArray(arr, n);

    quickSort(arr, 0, n - 1, ascending);
    printf("Ascending: ");
    printArray(arr, n);

    quickSort(arr, 0, n - 1, descending);
    printf("Descending: ");
    printArray(arr, n);

    quickSort(arr, 0, n - 1, evenFirst);
    printf("Even First: ");
    printArray(arr, n);

    return 0;
}


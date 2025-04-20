#include <stdio.h>

int doubleNum(int x) { return 2 * x; }
int squareNum(int x) { return x * x; }

void transformArray(int arr[], int n, int (*transform)(int)) {
    for (int i = 0; i < n; i++) {
        arr[i] = transform(arr[i]);
    }
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int arr[] = {1, 2, 3, 4};
    int n = 4;

    printf("original array: ");
    printArray(arr, n);

    printf("doubled: ");
    transformArray(arr, n, doubleNum);
    printArray(arr, n);

    int arr2[] = {1, 2, 3, 4};
    printf("squared: ");
    transformArray(arr2, n, squareNum);
    printArray(arr2, n);

    return 0;
}

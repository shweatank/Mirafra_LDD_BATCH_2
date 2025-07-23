#include <stdio.h>

void reverse(int *arr, int start, int end) {
    while (start < end) {
        int temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}

void rotateLeft(int arr[], int n, int d) {
    d = d % n;  // Important to normalize d

    reverse(arr, 0, d - 1);        // Reverse first d elements
    reverse(arr, d, n - 1);        // Reverse remaining n-d elements
    reverse(arr, 0, n - 1);        // Reverse the whole array
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7};
    int n = sizeof(arr) / sizeof(arr[0]);
    int pos = 5;
    printf("before roate elements \n");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    rotateLeft(arr, n, pos);
    printf("\nafter rotate elements by 5 pos\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }

    return 0;
}


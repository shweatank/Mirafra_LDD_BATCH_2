#include <stdio.h>
#include <stdlib.h>

int cmp_asc(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int cmp_desc(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

void print_arr(int arr[], int n) {
    for (int i = 0; i < n; ++i)
        printf("%d ", arr[i]);
    puts("");
}

int main() {
    int arr[] = {5, 2, 9, 1, 6};
    int n = sizeof(arr)/sizeof(arr[0]);

    qsort(arr, n, sizeof(int), cmp_asc);
    printf("Asc ");
    print_arr(arr, n);

    qsort(arr, n, sizeof(int), cmp_desc);
    printf("Des ");
    print_arr(arr, n);

    return 0;
}


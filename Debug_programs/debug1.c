#include <stdio.h>
#include <limits.h>

void process(int *arr, int size, int *evenSum, int *oddSum) {
    *evenSum = *oddSum = 0;
    int max_odd=0,sec_maxOddIndex=0;
    int maxEven = 0, maxEvenIndex = 0;
    int minOdd = 0, minOddIndex = 0;

    for (int i = 0; i <size; i++) {
        if (arr[i] % 2 == 0) {
            *evenSum += arr[i];
            if (arr[i] > maxEven) {
                maxEven = arr[i];
                maxEvenIndex = i;
            }
        } else {
            *oddSum += arr[i];
            if (arr[i] > minOdd)
            {
                max_odd=minOdd;
                minOdd = arr[i];
                sec_maxOddIndex=minOddIndex;
                minOddIndex = i;
            }
        }
    }

    // Swap logic
    printf("%d %d",maxEvenIndex,minOddIndex);
    if (maxEvenIndex || minOddIndex) {
        int temp = arr[maxEvenIndex];
        arr[maxEvenIndex] = arr[sec_maxOddIndex];
        arr[sec_maxOddIndex] = temp;
    }
}

int main() {
    int arr[] = {3, 10, 7, 4, 11, 6, 13};
    int evenSum, oddSum;
    int size = sizeof(arr) / sizeof(arr[0]);

    process(arr, size, &evenSum, &oddSum);

    printf("Sum of Even: %d\n", evenSum);
    printf("Sum of Odd: %d\n", oddSum);
    printf("Modified Array:\n");
    for (int i = 0; i < size; ++i)
        printf("%d ", arr[i]);
    printf("\n");

    return 0;
}

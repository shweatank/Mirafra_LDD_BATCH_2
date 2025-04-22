#include <stdio.h>

/* check and print the array elements which are even */

int is_even(int x) 
{
    return x % 2 == 0; 
}

int check_array(int *src, int *dst, int size, int (*check_even)(int)) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (check_even(src[i])) {
            dst[count++] = src[i];
        }
    }
    return count;
}

int main() {
    int size;
    printf("Enter the size: ");
    scanf("%d",&size);

    int arr[size];
    printf("Enter %d elements : ",size);
    for(int i = 0; i < size; i++) 
	scanf("%d",&arr[i]);

    int result[6], count;

    count = check_array(arr, result, size, is_even);

    printf("Even elements in Array : ");

    for (int i = 0; i < count; i++)
        printf("%d ", result[i]);
    printf("\n");

    return 0;
}


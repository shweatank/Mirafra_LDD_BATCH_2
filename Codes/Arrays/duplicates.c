#include <stdio.h>

void Sort(int arr[], int n)
{
    int temp, i, j;
    for(i = 0; i < n; i++)
    {
        for(j = i+1; j < n; j++)
        {
            if(arr[i] > arr[j])
            {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

int main()
{
    int arr[10] = {12, 45, 32, 56, 54, 89, 9, 32, 45, 12};
    
    Sort(arr, 10);
    
    printf("Sorted array: ");
    for(int i = 0; i < 10; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
    printf("Duplicate Elements:");
    for(int i=0;i<10;i++)
    {
        if(arr[i] == arr[i+1])
        {
            printf("%d ",arr[i]);
        }
        while(i<10-1 && arr[i]==arr[i+1])
        {
            i++;
        }
    }

    return 0;
}


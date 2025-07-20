#include <stdio.h>
int max(int arr[], int size, int nth)
{
    for(int i=0;i<nth;i++)
    {
        int max_index = i;
        for(int j=i+1;j<size;j++)
        {
            if(arr[j] > arr[max_index])
            {
                max_index = j;
            }
        }
        if(max_index != i)
        {
            int temp = arr[i];
            arr[i] = arr[max_index];
            arr[max_index] = temp;
        }
        if(i == nth -1)
        {
            printf("\nnth largest element %d", arr[i]);
        }
    }
}
int main()
{
    int arr[] = {55, 88, 33, 66, 11, 33};
    for(int i=0;i<6;i++)
    {
        printf("%d ",arr[i]);
    }
    max(arr,6, 3);
}

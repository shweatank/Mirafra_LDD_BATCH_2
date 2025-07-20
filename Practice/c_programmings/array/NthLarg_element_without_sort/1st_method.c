#include <stdio.h>
int max(int arr[], int size, int nth)
{
    int max = 0;
    for(int i=0;i<nth;i++)
    {
        int max_index = -1;
        for(int j=0;j<size;j++)
        {
            if(arr[j] > 0)
            {
                if(max_index == -1 || arr[j] > arr[max_index])
                {
                    max_index = j;
                }
            }
        }
        if(max_index != -1)
        {
            max = arr[max_index];
            arr[max_index] = -1;
        }
        else
        {
            return -1;
        }
    }
    printf("\nThe max index element is %d", max);
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

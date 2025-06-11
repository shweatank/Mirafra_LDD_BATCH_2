//WAP TO FIND SECOND LARGEST ELEMENT IN ARRAY

#include <stdio.h>

int sec_larg(int arr[], int size)
{
    int larg = 0, sec= 0;
    for(int i=0;i<size;i++)
    {
        if(arr[i] > larg)
        {
            sec = larg;
            larg = arr[i];
        }
        else if(arr[i] > sec && arr[i] < larg)
        {
            sec = arr[i];
        }
    }
    return sec;
}
int main()
{
    int size ;
    printf("Enter the size of an array : ");
    scanf("%d", &size);
    int arr[size];
    printf("enter the elements of an array \n");
    for(int i=0;i<size;i++)
    {
        scanf("%d", &arr[i]);
    }
    printf("elements of an array is : \n");
    for(int i=0;i<size;i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
    int sec = sec_larg(arr, size);
    printf("Array of second largest element is %d\n", sec);
}

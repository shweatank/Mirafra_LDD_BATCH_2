//WAP TO FIND THIRD LARGEST ELEMENT IN ARRAY

#include <stdio.h>

int third_larg(int arr[], int size)
{
    int larg = 0, sec= 0, third=0;
    for(int i=0;i<size;i++)
    {
        if(arr[i] > larg)
        {
            third = sec;
            sec = larg;
            larg = arr[i];
        }
        else if(arr[i] > sec && arr[i] < larg)
        {
            third = sec;
            sec = arr[i];
        }else if(arr[i] > third && arr[i] < sec)
        {
            third = arr[i];
        }
    }
    return third;
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
    int third = third_larg(arr, size);
    printf("Array of third largest element is %d\n", third);
}

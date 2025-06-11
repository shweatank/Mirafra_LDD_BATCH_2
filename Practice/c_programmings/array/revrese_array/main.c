//WAP TO REVERSE THE ARRAY ELEMENTS

#include <stdio.h>

int rev_arr(int arr[], int size)
{
    for(int i=0;i<size/2;i++)
    {
        int temp = arr[i];
        arr[i] = arr[size-i-1];
        arr[size-i-1] = temp;
    }
    
}
int main()
{
   int size;
   printf("Enter the size of an array : ");
   scanf("%d", &size);
   
   int arr[size];
   printf("Enter the array elements : \n");
   for(int i=0;i<size;i++)
   {
       scanf("%d", &arr[i]);
   }
   rev_arr(arr, size);
   printf("after reverse the array elements are : \n");
   for(int i=0;i<size;i++)
   {
       printf("%d ", arr[i]);
   }
}

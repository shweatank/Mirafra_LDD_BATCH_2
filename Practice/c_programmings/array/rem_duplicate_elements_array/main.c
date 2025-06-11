//WAP TO REMOVE DUPLICATE ELEMENT IN ARRAY

#include <stdio.h>

int remove_dup(int arr[], int size)
{
   int k=0;
   for(int i=0;i<size; i++)
   {
       int isdup =0;
       for(int j=i+1; j<size;j++)
       {
           if(arr[i] == arr[j])
           {
               isdup =1;
               break;
           }
       }
       if(!isdup)
       {
           arr[k] = arr[i];
           k++;
       }
   }
   printf("after removing the duplicate elements in the array is : \n");
   for(int i=0;i<k;i++)
   {
       printf("%d ", arr[i]);
   }
   return k;
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
   int s = remove_dup(arr, size);
   printf("\n");
   printf("In main function the removed array \n");
    for(int i=0;i<s;i++)
    {
        printf("%d ", arr[i]);
    }
}

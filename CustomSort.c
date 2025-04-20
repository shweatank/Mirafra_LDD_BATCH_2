#include<stdio.h>
int customSort(int* arr,int size,int (*cmp)(int,int));
int ascending(int a,int b)
{
    return a-b;
}
int descending(int a,int b)
{
    return b-a;
}
int evenFirst(int a,int b)
{
     // Put even numbers before odd numbers
    if((a%2==0) && (b%2!=0)) return -1;//
     if ((a % 2 != 0) && (b % 2 == 0)) return 1;
     
     //if both are even or both are odd
     return a-b;
}
void printarray(int* arr,int size)
{
    
    for(int i=0;i<size;i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
}
int main()
{
    int arr[100];
    int i,size;
    printf("Enter the array size : ");
    scanf("%d",&size);
    printf("Enter the array elements:");
    for(i=0;i<size;i++)
    {
        scanf("%d",&arr[i]);
    }
    printf("Original: ");
    printarray(arr,size);
    customSort(arr,size,ascending);
    printf("Ascending: ");
    printarray(arr,size);
    customSort(arr,size,descending);
    printf("descending: ");
    printarray(arr,size);
    // Even-first
    customSort(arr, size, evenFirst);
    printf("Even First: ");
    printarray(arr, size);

    return 0;
    
}

int customSort(int* arr,int size,int (*cmp)(int,int))
{
    int i,j;
    for(i=0;i<size-1;i++)
    {
        for(j=i+1;j<size;j++)
        {
            if(cmp(arr[i],arr[j])>0)
            {
                int temp=arr[i];
                arr[i]=arr[j];
                arr[j]=temp;
            }
        }
    }
}

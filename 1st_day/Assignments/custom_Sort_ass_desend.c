/*Custom Sort with Function Pointers

Problem Statement:
Implement a generic sorting function (similar to qsort) that accepts an array and a comparator function pointer to sort the array in different ways (ascending, descending, even-first, etc.).

Skills Used: Comparator functions, function pointer parameters.*/

#include <stdio.h>

int Ascending(int a, int b)
{
    return a-b;
}
int descending(int a, int b)
{
    return b-a;
}

int Even_First(int a, int b)
{
    if(a % 2 == 0 && b % 2 != 0)
    {
        return -1;
    }
    if(a % 2 != 0 && b % 2 == 0)
    {
        return 1;
    }
    
    return a-b;
}
int cumsort(int arr[], int n, int(* fptr)(int, int))
{
    for(int i=0;i<n-1;i++)
    {
        for(int j=i+1;j<n;j++)
        {
            if(fptr(arr[i], arr[j]) > 0)
            {
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}
int printArray(int arr[], int n)
{
    for(int i =0;i<n;i++)
    {
        printf("%d ",arr[i]);
    }
}
int main()
{
    int arr[10] = {8,7,9,5,4,3,2,1,10,6};
    int n  = sizeof(arr)/sizeof(arr[0]);
    printf("\tChoose any one of the option \n");
    printf("1. Ascending \n");
    printf("2. descending \n");
    printf("3. Even First \n");
    printf("select any one \n");
    int op;
    scanf("%d", &op);
    switch(op)
    {
        case 1:
        cumsort(arr, n , Ascending);
        break;
        case 2:
        cumsort(arr, n, descending);
        break;
        case 3:
        cumsort(arr, n, Even_First);
        break;
        
    }
    printf("sorted array \n");
    printArray(arr,n);
    return 0;
    
}

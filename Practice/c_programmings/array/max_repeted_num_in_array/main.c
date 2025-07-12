//find the maximum repeated number in given array:

#include <stdio.h>
#include <stdlib.h>
int max_repeated(int arr[], int size)
{
    int min = 0;
    int max = arr[0];
    for(int i=0;i<size;i++)
    {
        int c=1;
        for(int j=i+1;j<size; j++)
        {
            if(arr[i] == arr[j])
            {
                c++;
            }
        }
        
        if(c > min)
        {
            min = c;
            max = arr[i];
        }
    }
    printf("the max repeated number is %d and repeted to %d times\n", max, min);
    return max;
}
int main()
{
    int arr[] = {1,2,3,4,5,3,2,4,2,5,2,6,7,2,8};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    int max = max_repeated(arr, n);
    printf("max repeated number in a giver array is %d\n", max);
}


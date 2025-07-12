//Sort the array in Acending order:

#include <stdio.h>
#include <stdlib.h>
void maxr(int arr[], int n)
{
   for(int i=0;i<n;i++)
   {
       for(int j=i+1;j<n;j++)
       {
           if(arr[i] > arr[j])
           {
               int temp = arr[i];
               arr[i] = arr[j];
               arr[j] = temp;
           }
       }
   }
}

int main(){
        int a[]={1,2,3,9,4,5,6,3,2,2,4,5,7};
    int n=sizeof(a)/sizeof(a[0]);
    maxr(a,n);
    for(int i=0;i<n;i++)
    {
        printf("%d ", a[i]);
    }
}

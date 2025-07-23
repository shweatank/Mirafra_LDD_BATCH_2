#include <stdio.h>


int main()
{
    int arr[]={1,2,1,2,3,2,1,1};
    int n=sizeof(arr)/sizeof(arr[0]);
   for (int i=0;i<n;i++){
    int count=0;
    for(int j=i+1;j<n;j++){
        if(arr[i]==arr[j]){
        count++;
        }
    }
    if(count==0){
        printf("%d",arr[i]);
    }
} 
}

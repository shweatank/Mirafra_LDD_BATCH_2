#include <stdio.h>

int main() {
    int arr[] = {5, 3, 2, 4, 1};
    int n = sizeof(arr) / sizeof(arr[0]);

    int i=0;
    while(i<n-1)
    {
        if(arr[i]  >arr[i+1])
        {
            int temp = arr[i];
            arr[i] = arr[i+1];
            arr[i+1] = temp;
            
            i=0;
        }
        else
        {
            i++;
        }
    }

    // print sorted array
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }

    return 0;
}


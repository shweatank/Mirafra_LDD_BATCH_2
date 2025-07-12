//Move the even elements to right side in given Array:

#include <stdio.h>

int main()
{
   int a[]={1,2,3,4,3,2,1,6,45,6,6};
   int size = sizeof(a) / sizeof(a[0]);
   int index = 0;
   
   for(int i=0;i<size;i++)
   {
       if(a[i] % 2 != 0)
       {
           int temp = a[index];
           a[index] = a[i];
           a[i] = temp;
           index++;
       }
   }
   
   for(int i=0;i<size;i++)
   {
       printf("%d ", a[i]);
   }
}

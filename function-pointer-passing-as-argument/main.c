#include<stdio.h>
#include<stdlib.h>
int comparator_ascending(const void *a,const void *b)
{
    return(*(int *)a-*(int *)b);
}
int comparator_descending(const void *a,const void *b)
{
    return(*(int *)b-*(int *)a);
}
void bubblesort(int a[],int n,int(*comparator)(const void *,const void *))
{
    int i,j;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n-1-i;j++)
        {
            if(comparator(&a[j],&a[j+1])>0)
            {
                int temp=a[j];
                a[j]=a[j+1];
                a[j+1]=temp;
            }
        }
    }
}
int main()
{
    int a[]={3,2,1,5,4};
    int n=sizeof(a)/sizeof(a[0]);
    bubblesort(a,n,comparator_ascending);
    for(int i=0;i<n;i++)
    {
        printf("%d",a[i]);
    }
    printf("\n");
      bubblesort(a,n,comparator_descending);
    for(int i=0;i<n;i++)
    {
        printf("%d",a[i]);
    }
}
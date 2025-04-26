#include<stdio.h>


int main(){
  int a[]={1,3,2,6,7,4,9,0};
  int n=sizeof(a)/sizeof(a[0]);
  
  for(int i=0;i<n;i++){
      for(int j=i+1;j<n;j++){
          if(a[i]>a[j]){
              int t;
              t=a[i];
              a[i]=a[j];
              a[j]=t;
          }
      }
  }
  for(int i=0;i<n;i++){
      printf("%d ",a[i]);
  }
}

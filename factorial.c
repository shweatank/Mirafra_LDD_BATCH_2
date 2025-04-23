  1  #include<stdio.h>
  2 #include<unistd.h>
  3 #include<sys/stat.h>
  4 #include<stdlib.h>
  5 #include<sys/wait.h>
  6 #include<sys/mman.h>
  7 //int res[3];
  8 int factorial(int n)
  9 {
 10         int fact=1;
 11         while(n>0)
 12         {
 13                 fact=fact*n;
 14                 n--;
 15         }
 16         return fact;
 17 }
 18 int main()
 19 {
 20         int arr[3]={1,2,3};
 21         int *res = mmap(NULL, 3 * sizeof(int),
 22            PROT_READ | PROT_WRITE,    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
 24         //static int res[3]={0};
 25         for(int i=0;i<3;i++)
 26         {
 27                 pid_t p=fork();
 28                 if(p==0)
 29                 {
 30                    res[i]=factorial(arr[i]);
 31                   // perintf("%d ",res[i]);
 32                    exit(0);
 33                 }
 34                 if(p>0)
 35                 {
 36                         wait(NULL);
 37                 }
 38         }
 39         for(int i=0;i<3;i++)
 40         {
               printf("%d ",res[i]);
 42         }
 43         printf("\n");
 44         munmap(res,3*sizeof(int));
 45         return 0;
 46 }

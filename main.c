#include<stdio.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
int factorial(int n)
{
    int res=1;
    for(int i=1;i<=n;i++)
    {
        res*=i;
    }
    return res;
}
int main()
{
    int a[]={3,4,5,6};
    int n=sizeof(a)/sizeof(a[0]);
    key_t key=IPC_PRIVATE;
    pid_t pid;
   int shmid=shmget(key,1024,IPC_CREAT|0666);
    int *shmptr=(int *)shmat(shmid,NULL,0);
    
    for(int i=0;i<n;i++)
    {
        pid=fork();
        if(pid<0)
        {
            printf("error:");
        }
        else
        {
        shmptr[i]=factorial(a[i]);
        shmdt(shmptr);
        }
    }
    for(int i=0;i<n;i++)
     wait(NULL);
     
    for(int i=0;i<n;i++)
    {
        printf("%d=%d",a[i],shmptr[i]);
    }
    shmdt(shmptr);
    shmctl(shmid,IPC_RMID,NULL);
}
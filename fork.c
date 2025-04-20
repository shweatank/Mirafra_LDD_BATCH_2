#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>

int fact(int n){
	int z=1;;
	for(int i=1;i<=n;i++){
	     z=z*i;
	}
	return z;
}

int main(){
	
	int number[]={4,8,4,6,5};
	int size=5,ret;
	//int arr[size];
	
	int shmid=shmget(IPC_PRIVATE,sizeof(int)*size,IPC_CREAT| 0644);
	if(shmid<0){
	perror("error");
	}
	
	int *arr=shmat(shmid,NULL,0);
	if(arr==-1){
	perror("shmat failed\n");
	}
		
	for(int i=0;i<size;i++){
		ret=fork();
		if(ret<0){
		perror("error\n");
		exit(1);
		}
		else if(ret==0){
		arr[i]=fact(number[i]);  //children
		exit(0);
		}
		else
		wait(NULL);   //parent
	}
	
	//for(int i=0;i<size;i++){
	//wait(NULL);
	//}
	
	printf("number are given below\n");
	for(int i=0;i<size;i++){
	printf("%d\t",number[i]);
	}
	
	printf("\nfinal array of factorial given below\n");
	for(int i=0;i<size;i++){
	printf("%d\t",arr[i]);
	}
	printf("\n");
	
	return 0;

}

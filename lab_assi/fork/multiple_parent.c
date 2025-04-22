#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void parent_fun(){
	printf("parent pid=%d\n",getpid());
	for(int i=0;i<5;i++){
		printf("parent is working...%d\n",i);
		sleep(1);
	}
	printf("parent finished\n");
}

int main(){
	pid_t pid=fork();
	sleep(1);

	if(pid<0){
		perror("error");
		exit(1);
	}
	else if(pid==0){
		printf("child pid=%d\n",getpid());
		execlp("/usr/bin/echo","echo","child say hello from exec",(char*)NULL);

		perror("failed");
		exit(1);
	}
	else{
	//	printf("parent pid=%d\n",getpid());
		wait(NULL);
		parent_fun();
	}
	
	return 0;

}



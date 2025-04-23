#include<stdio.h>
#include<unistd.h>

int main(){
	int arr[2];
	pipe(arr);
pid_t pid=fork();
if(pid==0){
write(arr[1],"hello",5);
}
else{
	char buf[6];
read(arr[0],buf,5);
printf("in parent process%s\n",buf);
}
}

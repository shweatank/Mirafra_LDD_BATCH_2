#include<stdio.h>
#include<unistd.h>

int main(){
int arr[2];
if(pipe(arr)<0){
perror("pipe");
return 0;
}
pid_t pid=fork();
if(pid<0){
perror("fork:");
return 0;
}
if(pid==0){
close(arr[0]);
dup2(arr[1],1);
close(arr[1]);
execl("/bin/ls","ls","-l",(char*)NULL);
perror("execl");
return 0;
}
else{
close(arr[1]);
char buf[1000];
int bytes=read(arr[0],buf,sizeof(buf));
if(bytes>0){
buf[bytes]='\0';
}
close(arr[0]);
printf("the command is :\n%s",buf);
}
}

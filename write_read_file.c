#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
int main(){
int fd=open("text",O_RDWR);
char msg[10]="hello";
int size=write(fd,msg,5);
lseek(fd,0,SEEK_SET);
char buf[10];
int size1=read(fd,buf,5);
buf[6]='\0';
int size2=write(1,buf,5);
close(fd);
}

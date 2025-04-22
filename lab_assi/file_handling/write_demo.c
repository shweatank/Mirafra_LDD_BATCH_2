#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>

int main(){
	FILE *fp,*fk;
	fp=open("try1.txt",O_WRONLY);

	if(fp == NULL) {
        perror("error");
        return 1;
    }
    char data[] = "shubham katole";
    write(fp,data,strlen(data));
    close(fp);
            	
    fk=open("try1.txt",O_RDONLY);
        
        if(fk == NULL) {
        perror("error");
        return 1;
    }

    char buff[50];
    int z=read(fk,buff,sizeof(buff));
    buff[z]='\0';
    printf("read data=%s\n",buff);
    close(fk);
}



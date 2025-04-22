#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(){
	FILE *fp;
	fp=fopen("try.txt","w");

	if(fp==NULL){
		perror("error");
		return 1;
	}
	char str[]="shubham";
	fwrite(str,1,strlen(str),fp);
	fclose(fp);

	FILE *fk;
	fk=fopen("try.txt","r");
	if(fk==NULL){
                perror("error");
                return 1;
        }
	char buff[50];
	int k=fread(buff,1,sizeof(buff),fk);
	buff[k]='\0';
	printf("read data=%s", buff);
	fclose(fk);
}

#include<stdio.h>
#include<stdlib.h>
int main()
{

	FILE *fp;
	fp=fopen("q2.txt","r");//  if w+ and r+ then changes in file content
	
	if(fp==NULL)
	{
		perror("null");
		exit(1);
	}
	char *ch="shubham";
	while(*ch!=NULL)
	{
		fputc(*ch,fp);
		ch++;
	}
	
	char cd;
	while((cd=getc(fp))!=EOF){
		printf("%c",ch);
		}
		
	fclose(fp); // q2 file contain same thing which u print....
}

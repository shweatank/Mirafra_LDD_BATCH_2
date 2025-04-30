#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
void log_message(FILE *fp,const char *level,const char *message)
{
	time_t now;
	time(&now);
	char *time_stp=ctime(&now);
	time_stp[strcspn(time_stp,"\n")]='\0';
	printf("%s %s %s",time_stp,level,message);
	if(fp!=NULL)
	{
		fprintf(fp,"%s %s %s",time_stp,level,message);
	}
}
int main()
{
	FILE *fp;
	fp=fopen("file.txt","a");
	if(fp==NULL)
	{
		printf("error");
		exit(0);
	}
	log_message(fp,"info"," program execution started");
	int a,b;
	char op;
	printf("enter a and b values and operation to be performed:");
	scanf("%d %c %d",&a,&op,&b);
	switch(op)
	{
		case '+':log_message(fp,"info","addition is stared\n");
			 printf("add=%d\n",a+b);
			 log_message(fp,"info","addition is completed\n");
			 break;
		case '-':log_message(fp,"info","subtraction is started\n");
			printf("sub=%d\n",a-b);
			log_message(fp,"info","subtraction is completed\n");
			break;
		case '*':log_message(fp,"info","multiplication is started\n");
			 printf("mul=%d\n",a*b);
			 log_message(fp,"info","multiplication is completed\n");
			 break;
		default:log_message(fp,"error","invalid selection..\n");
	}
	log_message(fp,"info","program execution is completed..\n");
	fclose(fp);
}

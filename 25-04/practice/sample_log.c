#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

void log_message(const char *level,const char *message)
{
	time_t now;
	time(&now);
	char *time_sp=ctime(&now);
	time_sp[strcspn(time_sp,"\n")]='\0';
	printf("[%s] [%s] %s\n",time_sp,level,message);
}

int main()
{
	log_message("info","program started");
	int a,b,r;
	printf("enter  a and b:");
	scanf("%d %d",&a, &b);
	if(b==0)
	{
		log_message("error", "division by zero not possible");
	}
	else
	{
		r=a/b;
		printf("result:%d\n",r);
	}
	log_message("info", "program ended");
}

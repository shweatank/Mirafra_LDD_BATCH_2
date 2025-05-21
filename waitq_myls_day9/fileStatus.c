#include"headers.h"
int main(int argc,char **argv)
{
	struct stat  buf;
	char dateString[20],timeString[20];
	struct tm *tmptr;
	if(argc<2)
	{
		printf("error:file not supplied\n");
		exit(0);
	}
			
	if(stat(argv[1],&buf)==-1)
	{
		perror("stat");
		exit(0);
	}
	
	printf("size:%u\n",(unsigned int)buf.st_size);
	printf("linkCnt=%u\n",(unsigned int)buf.st_nlink);
	printf("permission:%o\n",(unsigned int)buf.st_mode);
//	printf("modification time:%u\n",(unsigned int)buf.st_mtime);
	tmptr=localtime(&buf.st_mtime);
	strftime(dateString,20,"%F",tmptr);
	strftime(timeString,20,"%T",tmptr);
	printf("%s %s\n",dateString,timeString);

}

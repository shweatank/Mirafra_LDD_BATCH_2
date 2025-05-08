#include<stdio.h>
#include<string.h>
int main(int argc, char *argv[])
{
		if(argc > 2 || argc < 2)
		{
				return 1;
		}
		char ch;
		FILE *fp;
		int c=strcmp(argv[1],"file.txt");
		if(c==0)
		{
		//FILE *fp;
		fp=fopen(argv[1],"r");
		if(fp!=NULL)
		{
				for(;(ch=fgetc(fp))!=EOF;)
				{
						printf("%c",ch);
				}
		}
		}
		fclose(fp);
		return 0;
}

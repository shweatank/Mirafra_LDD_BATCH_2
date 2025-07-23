#include<stdio.h>
#include<stdlib.h>
int main()
{
		FILE *fp;
		fp=fopen("kavya.txt","w");
		fwrite(fp,1,1,"kavya");
		char buf[5];
		fread(fp,1,1,buf);
		printf("%s\n",buf);
		return 0;
}

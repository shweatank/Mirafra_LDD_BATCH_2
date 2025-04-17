#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *fp;
	char name[] = "vishnu";
	char buff[50];
	fp = fopen("my_name.txt", "w");
	if(fp == NULL)
	{
		printf("file open failed \n");
	}
	fprintf(fp , "%s", name);
	fclose(fp);

	fp = fopen("my_name.txt", "r");
	fscanf(fp, "%s", buff);
	printf("%s",buff);
	fclose(fp);
	return 0;

}

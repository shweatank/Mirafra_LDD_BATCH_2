#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
	FILE *fp;
	const char *filename = "My_name.txt";
	const char *msg = "Hello this is Vishnu \n this is a file operation demo.";
	
	//step 1: open file for writing 
	fp = fopen(filename , "w");
	if(fp == NULL)
	{
		perror("error in opening the file");
		return 1;
	}

	//step 2 write message
	fwrite(msg, sizeof(char), strlen(msg), fp);

	//step 3 close the file
	
	fclose(fp);

	//step 4 open the file 
	 fp = fopen(filename , "r");
        if(fp == NULL)
        {
                perror("error in opening the file");
                return 1;
        }
	
	//step 6 read the contents from the file
	char buff[100];
	size_t bytesread = fread(buff, sizeof(char), sizeof(buff)-1, fp);
	printf("File contents \n%s\n", buff);

	//step 6 close the file
	fclose(fp);
	return 0;
}

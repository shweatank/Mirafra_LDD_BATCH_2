//using fopen file operations
/*#include<stdio.h>
#include<string.h>
int main()
{
	FILE *fp ; 
	char* filename = "temp.txt";
	fp=fopen(filename,"w");
	if(fp==NULL)
	{
		perror("Error opening for writing");
		return 1;
	}

	char* str ="Arya";
	fwrite(str,sizeof(char),strlen(str),fp);
	fclose(fp);

	fp=fopen(filename,"r");
	if(fp ==NULL)
	{
		perror("Error opeming file for reading");
		return 1;
	}

	//read content and printf
	char buffer[100];
	size_t bytedRead = fread(buffer,sizeof(char),sizeof(buffer)-1,fp);
	buffer[bytedRead]='\0';
	printf("File comntent\n%s\n:",buffer);
	fclose(fp);

        return 0;


}*/


//using open() system calls
#include<stdio.h>
#include<string.h>
 #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

int main()
{
        FILE *fp ;
        char* filename = "TTemp.txt";
	
        fp=open(filename,O_CREAT);
}
    /*    if(fp==NULL)
        {
                perror("Error opening for writing");
                return 1;
        }

        char* str ="Arya";
        fwrite(str,sizeof(char),strlen(str),fp);
        fclose(fp);
i
        fp=fopen(filename,"r");
        if(fp ==NULL)
        {
                perror("Error opeming file for reading");
                return 1;
        }

        //read content and printf
        char buffer[100];
        size_t bytedRead = fread(buffer,sizeof(char),sizeof(buffer)-1,fp);
        buffer[bytedRead]='\0';
        printf("File comntent\n%s\n:",buffer);
        fclose(fp);

        return 0;


}*/



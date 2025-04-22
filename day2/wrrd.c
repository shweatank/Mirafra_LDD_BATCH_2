#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc ,char **argv) 
{
	int wr ,rd;

	char data[] = "pora p....",rdata[100];
	int br;
	if(argc!=3)
	{
		write(STDERR_FILENO,"ENTER VALID FILE NAMES\n",22);
	}

	rd= open(argv[1], O_CREAT|O_RDONLY,0666);
	if (rd == -1) 
	{
		printf(" rd Error opening the file\n");
		return 1;
	}

	read(rd,rdata,sizeof(rdata));
	close(rd);

	printf("rdfd closed successfully.\n");

	wr = open(argv[2], O_WRONLY|O_CREAT,0666);

	if (wr == -1) {
		printf("wr Error opening the file\n");
		return 1;
	}
	write(wr,data,13);

	//  br= read(rd,rdata,13);
	printf("%s\n",rdata);
	close(wr);
	printf("wrfd closed successfully.\n");

}

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
int main()
{
	int fp;
	char name[] = "This is vishnuvardhan";
	char buff[100];

	fp = open("my_name.txt",O_WRONLY | O_CREAT | O_TRUNC );

	write(fp, name, sizeof(name));
	close(fp);

	fp = open("my_name.txt",O_RDONLY);

	size_t byteread = read(fp, buff, sizeof(buff)-1);
	if(byteread == -1)
	{
		perror("reading failed");
		return 1;
	}
	printf("File contents \n %s \n", buff);

	close(fp);
	return 0;
}


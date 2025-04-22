#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>

int main(int argc, char *argv[]){
	
	if(argc !=3){
		perror("exit");
		return 1;
	}

	char *cret=argv[1];
	char *str=argv[2];

	//     int z=open(cret,O_WRONLY | O_CREAT | O_TRUNC, 0644);i
          int z=open(cret,O_WRONLY | O_CREAT| O_TRUNC, 0644);
	if(z==-1){
		 perror("exit");
                return 1;
        }       
	
	if(write(z,str,strlen(str))==-1){
			 perror("exit");
	                return 1;
        }
	
	close(z);
}









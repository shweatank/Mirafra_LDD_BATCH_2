#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include<string.h>
extern int errno;
int main() {
	int bytes = 0;
 	int driver_fd,txtfile_fd = 0;
        char output[512];
	char input[100];
        int count = 0;
     driver_fd  = open("/dev/driverfile", O_RDWR);
     if(driver_fd  < 0) {
        perror("Failed to open device driver device");
        return EXIT_FAILURE;
      }
     
      txtfile_fd  = open("file7.txt",O_CREAT | O_RDWR,0666);
      if(txtfile_fd < 0){
	    printf("Error Number % d\n", errno);
	    return -1;
      }
      printf("enter the string : \n");
      fgets(input, sizeof(input), stdin);
      write(driver_fd, input, strlen(input));
      while(1){	          
           count++;
           printf("count value = %d",count);
           bytes =  read(driver_fd, output, sizeof(output));
           if(bytes != 0){
        	bytes = write(txtfile_fd, output, bytes); 
	        if(bytes == 0){
	             printf("failed to write into text file \n");
	         }else {
	    	      printf("successfully written logs  into text file");
	             break;	
		  }
            }
            
          //sleep(1);        
      }
     
     // Clean up and close the file
     close(txtfile_fd);
     close(driver_fd);
     return 0;
}

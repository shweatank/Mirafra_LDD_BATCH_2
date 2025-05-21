#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main(){
int fd;
    char input[100];
    char output[2048];
fd = open("/dev/dup_dmesg", O_RDWR);
if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }
while(1){
read(fd, output, sizeof(output));
if(output[0]=='\0'){
continue;
}
puts(output);
}
close(fd);


}

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char input[100];
    char output[100];

    fd = open("/sys/kernel/my_sysfs/hello", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Enter string:\n");
    fgets(input, sizeof(input), stdin);

    write(fd, input, strlen(input));
    lseek(fd, 0, SEEK_SET);     

    int z=read(fd, output, sizeof(output)-1);
    output[z] = '\0'; 
    printf("From kernel string : %s", output);

    close(fd);
    return 0;
}

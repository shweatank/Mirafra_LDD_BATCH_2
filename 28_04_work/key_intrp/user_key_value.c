#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_LOCATION "/dev/arithmatic_keys"

struct data {
    int num1, num2, result;
    char key;
};

int main() {
    int fd;
    char write_buf[200], read_buf[200];
    struct data d;

    printf("Enter 2 numbers: ");
    scanf("%d %d",&d.num1,&d.num2);

    fd = open(DEVICE_LOCATION, O_WRONLY);
    if (fd < 0) {
        perror("open fails.");
        return 1;
    }

    while(1){
	d.key = getchar();
    	write(fd, &d, sizeof(struct data));
	read(fd, &d, sizeof(struct data));
    	printf("Result from kernel: %d\n", d.result);
    }

    close(fd);

    return 0;
}

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char input[100];
    char output[100];

    fd = open("/dev/charcal", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Enter operation (e.g., 5 + 3): ");
    fgets(input, sizeof(input), stdin);

    write(fd, input, strlen(input));
	while(1){
		printf("press the key\n");
    read(fd, output, sizeof(output));
    printf("From kernel: %s", output);
    sleep(10);

	}

    close(fd);
    return 0;
}


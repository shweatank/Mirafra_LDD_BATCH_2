#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVICE "/dev/simple_calculator1"
#define MAGIC_NUM 100
#define IOCT_SET_DATA _IOW(MAGIC_NUM, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, int *)

struct calc_data {
    int num1;
    int num2; 
    char opeartion;
    int result;
};
int main()
{
    int fd, result;
    struct calc_data data;
    
    fd = open(DEVICE, O_RDWR);
    if(fd < 0)
    {
        perror("open failed");
        return EXIT_FAILURE;
    }
    printf("Enter calculation (e.g: 5 + 6) \n");
    scanf("%d %c %d", &data.num1, &data.opeartion, &data.num2);
    //send data to kernel
    IOCTL(fd, IOCT_SET_DATA, &data);

    //read data from kernel
    IOCTL(fd, IOCTL_GET_RESULT, &result);

    printf("Result %d\n", result);
    close(fd);
    return 0;
    
}
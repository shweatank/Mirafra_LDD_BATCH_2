#include<unistd.h>
#include<sys/ioctl.h>
#include<string.h>

#define DEVICE "/dev/calculator"
#define MAGTC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM,0,struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM,1,int *)

strcut calc_data{
int num1;
int num2;
char operation;
int result;
};

int main(){
int fd,result;
struct calc_data calc;
fd= open(DEVICE,O_RDWR);
if(fd<0){
perror("failed to open the driver");
return EXIT_FAILURE;
}

printf("Enter calculation (e.g., 10 + 5):");
scanf("%d %c %d", &calc.num1,&calc.operation,&calc.num2);

ioctl(fd,IOCTL_GET_RESULT,&result);
printf("Result: %d\n",result);
close(fd);
return 0;
}

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<string.h>

#define DEVICE "/dev/simple_calculator"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA_LOW(MAGIC_NUM,0,struct calc_data*)
#define IOCTL_GET_RESULT_LOW(MAGIC_NUM,1,int*)

struct calc_data{

	int num1;
	int num2;
	char operation;
	int result;};
	
int main(){

	int fd,result;
	struct calc_data calc;
	
	fd = open(DEVICE,O_RDWR);
	if(fd< 0){
		perror("fail to open");
		return EXIT_FAILURE;
		
		}
		
	printf("enter calculation eg 10 +7:");
	scanf("%d %c %d",&calc.num1,&calc.operation,&calc.num2);
	
	ioctl(fd,IOCTL_SET_DATA,&calc);
	
	ioctl(fd,IOCTL_GET_RESULT,&result);
	
	printf("result: %d\n",result);
	
	close(fd);
	return 0;
	
	
	}

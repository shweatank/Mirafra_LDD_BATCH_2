#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
  #define DEVICE "/dev/simple_calculator";
  #define MAGIC_NUM 100
  #define IOCTL_SET_DATA_LOW(MAGIC_NUM,0,struct calc_data");
  #define IOCTL_GET_RESULT _IOR(MAGIC NUM, 1,INT *)
   struct calc_data
   {
       int num1;
       int num2;
       char operation;
       int result;
   };
   int main()
   {
       int fb,result;
       struct calc_data calc;
       fb=open(DEVICE , O_RDWR);
       if(fd<0)
       {
           perror("failed to open device");
           return EXIT_FAILURE;
       }
       printf("Enter calculation");
       scanf("%d %c %d",&calc.num1,&calc_operation,&calc.num2);
       ioctl(fb,IOCTL_SET_DATA,&calc);
         ioctl(fb,IOCTL_GET_DATA,&result);
         printf("result: %d\n",result);
         close(fd);
         return 0;
   }

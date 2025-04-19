/* 
1. operation using 3 threads 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>    
#include <unistd.h>   
#include <string.h> 
int result,input_ready,calc_ready;
int a, b;
char op;
void *take_input(){
    printf("Enter two numbers : ");
    scanf("%d %d",&a,&b);
    printf("Enter the operation: ");
    scanf(" %c",&op);
    input_ready = 1;
    return NULL;
}
void *operation(){
    while(input_ready == 0);
    switch(op){
        case '+':result = a+b;break;
        case '-':result = a-b;break;
        case '*':result = a*b;break;
        case '/':result = a/b;break;
        default:printf("Invalid operator\n");
        
    }
    calc_ready = 1;
    return NULL;
}
void *write_result(){
    while(calc_ready ==0 );
    int fd = open("ex.txt",O_WRONLY|O_CREAT,0644);
    if(fd == -1){
        perror("file");
        return 0;
    }
    char str[20];
    sprintf(str,"%d",result);
    write(fd,str,strlen(str));
    printf("Result written into file successfully\n");
    return NULL;
}


int main()
{
    pthread_t thread1,thread2,thread3;
    pthread_create(&thread1,NULL,take_input,NULL);
    pthread_create(&thread2,NULL,operation,NULL);
    pthread_create(&thread3,NULL,write_result,NULL);
    
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    pthread_join(thread3,NULL);
    return 0;
}




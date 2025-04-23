#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/stat.h>
#include<fcntl.h>
int n1,n2,res;
char ch;
pthread_mutex_t l;
void *input()
{
		pthread_mutex_lock(&l);
		printf("Enter an expression:\n");
		scanf("%d %c %d",&n1,&ch,&n2);
		pthread_mutex_unlock(&l);
}
void *operation()
{
		pthread_mutex_lock(&l);
		switch(ch)
		{
				case '+':
						res=n1+n2;
						break;
				case '-':
						res=n1-n2;
						break;
				case '*':
						res=n1*n2;
		         		break;
				case '/':
						if(n2!=0)
						{
								res=n1/n2;
						}
						else
						{
								printf("division not possible\n");
						}
						break;
		}
		pthread_mutex_unlock(&l);
}
void *write_res()
{
		pthread_mutex_lock(&l);
		int fp=open("res.txt",O_CREAT | O_RDWR ,0644);
		int buf[1];
		buf[0]=res;
		write(fp,buf,sizeof(buf));
		lseek(fp,0,SEEK_SET);
		int r[1];
		read(fp,r,sizeof(r));
				printf("%d\n",r[0]);
				pthread_mutex_unlock(&l);
}
int main()
{
		pthread_t t1;
		pthread_t t2;
		pthread_t t3;
		pthread_mutex_init(&l,NULL);
		pthread_create(&t1,NULL,input,NULL);
		pthread_create(&t2,NULL,operation,NULL);
		pthread_create(&t3,NULL,write_res,NULL);
		pthread_join(t1,NULL);
		pthread_join(t2,NULL);
		pthread_join(t3,NULL);
		pthread_mutex_destroy(&l);
}

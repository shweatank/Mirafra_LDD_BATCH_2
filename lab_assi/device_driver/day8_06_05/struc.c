#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct data{
	int no;
	char ch;
	char name[10];
};

int main(){
    char buffer[50];
    struct data s2;
    struct data s1={1,'a',"shubham"};

    printf("no=%d character=%c name=%s\n", s1.no,s1.ch,s1.name);

    memcpy(buffer,&s1,sizeof(s1));
    printf("%d",buffer[0]);
    for (int i = 1; i < sizeof(s1); i++) {
        printf("%c ", buffer[i]);
    }
    printf("\n");

    memcpy(&s2,buffer,sizeof(buffer));
    printf("s1.no=%d\n",s2.no);
    printf("s1.char=%c\n",s2.ch);
    printf("s1.name=%s\n",s2.name);
    //strcpy(buffer,s1.name);
   


    return 0;

   }


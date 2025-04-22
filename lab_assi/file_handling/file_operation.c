#include<stdio.h>
#include<stdlib.h>
int main()
{
	char data[100];
        printf("enter the data\n");
        scanf("%[^\n]s",data);
        puts(data);

        FILE *fp,*fk;
        fp=fopen("q2.txt","w");

        if(fp==NULL)
        {
                perror("null");
                exit(1);
        }
        char *ch="shubham";
        while(*ch!=NULL)
        {
               fputc(*ch,fp);
                ch++;
        }

	char cp;
        fk=fopen("q2.txt","r");
	while((cp=getc(fk))!=EOF)
	{
		printf("%c",cp);
	}
        fclose(fk); 
}

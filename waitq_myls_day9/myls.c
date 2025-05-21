#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<time.h>
#include<grp.h>
#include<pwd.h>

//typedef struct __dirstream DIR;
int main(int argc,char *argv[])
{
	char s[100];
	strcpy(s,".");
        DIR *p;
        struct dirent *d;
	 char dateString[20],timeString[20];
        struct tm *tmptr;
	struct stat buf;
	if(stat(s,&buf)==-1)
        {
                perror("stat");
                exit(0);
        }

        p=opendir(s);
	if(argc==1||(!strcmp(argv[1],"-a")))
	{
		while((d=readdir(p))!=NULL)
		{
			if(!(strcmp(d->d_name,"."))||!(strcmp(d->d_name,"..")))
			{
				continue;
			}
			else
				printf("%s\t",d->d_name);
		}
	}
	else if((!strcmp(argv[1],"-l"))||(!strcmp(argv[1],"-g")))
	{
		struct group *grp;
		int i;
		struct passwd *pwd;
		while((d=readdir(p))!=NULL)
                {
			if(stat(d->d_name,&buf)==-1)
			{
				perror("stat");
				exit(0);
			}
                        if(!(strcmp(d->d_name,"."))||!(strcmp(d->d_name,"..")))
                        {
                                continue;
                        }
                        else
			{
				grp=getgrgid(buf.st_gid);
				pwd=getpwuid(buf.st_uid);
				switch (buf.st_mode & S_IFMT)
           			{
                 		case S_IFBLK:  printf("b");
                                 		break;
                 		case S_IFCHR:  printf("c");
                                		break;
                 		case S_IFDIR:  printf("d");
                                		break;
                 		case S_IFIFO:  printf("p");
                                		break;
                		case S_IFLNK:  printf("s");
                                		break;
                 		case S_IFREG:  printf("-");
                                 		break;
                 		case S_IFSOCK: printf("s");
                                		break;
                 		default:       printf("?");
                                		break;
           			}
			for(i=0;i<9;i+=3)
			{
				if(buf.st_mode&(1<<(8-i)))
					printf("r");
				else 
					printf("-");
				if(buf.st_mode&(1<<(7-i)))
                                        printf("w");
                                else
                                        printf("-");

				if(buf.st_mode&(1<<(6-i)))
                                        printf("x");
                                else
                                        printf("-");
			}




			printf(" ");
			printf("%lu ",buf.st_nlink);
			printf("%s ",grp->gr_name);
			if(!strcmp(argv[1],"-l"))
			printf("%s ",pwd->pw_name);
			printf(" ");
			printf("%5lu  ",buf.st_blocks);
			tmptr=localtime(&buf.st_mtime);
        		strftime(dateString,20,"%F",tmptr);
        		strftime(timeString,20,"%T",tmptr);
        		printf("%s %s ",dateString,timeString);
			printf("%s\n",d->d_name);
			}
		}
	}
	else if(!strcmp(argv[1],"-i"))
	{
		while((d=readdir(p))!=NULL)
                {
                        if(stat(d->d_name,&buf)==-1)
                        {
                                perror("stat");
                                exit(0);
                        }
                        if(!(strcmp(d->d_name,"."))||!(strcmp(d->d_name,"..")))
                        {
                                continue;
                        }
                        else
                        {
				printf("%lu ",buf.st_ino);
				printf("%s\n",d->d_name);
			}
		}
	}
	else if(!strcmp(argv[1],"-s"))
	{
		while((d=readdir(p))!=NULL)
                {
                        if(stat(d->d_name,&buf)==-1)
                        {
                                perror("stat");
                                exit(0);
                        }
                        if(!(strcmp(d->d_name,"."))||!(strcmp(d->d_name,"..")))
                        {
                                continue;
                        }
                        else
                        {
                              printf("%lu  ",buf.st_nlink);
			      printf("%s\n",d->d_name);
			    
			}
		}
	}

}                  

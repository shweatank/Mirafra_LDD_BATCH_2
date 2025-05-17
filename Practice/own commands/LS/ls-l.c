#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<grp.h>
#include<pwd.h>
#include<time.h>
#define NORMAL_COLOR  "\x1B[0m"
#define GREEN  "\x1B[32m"
#define BLUE  "\x1B[34m"
char arr[11]="----------";
char check( int  per){
	//	printf("per:%d  ",per);
	for(int i=0;i<10;i+=3){
		if((per>>i)&1){
			return 1;
		}
	}
	return 0;
}
struct st{
char *dup;
char *fname;
unsigned int hard_links;
char *g_name;
char *u_name;
unsigned int size;
unsigned char *time;
char symbool;
};
int cnt=0;
void print(struct st *db,int n,int n1){
for(int i=0;i<cnt;i++){
	if(db[i].symbool=='d'){
		int h=n-strlen(db[i].u_name)+1;
		int j=(n1-strlen(db[i].g_name))+1;
		printf("%s%s  %2d %s%*s %s%*s%8d %s %s%s\n",NORMAL_COLOR,db[i].dup,db[i].hard_links,db[i].u_name,h,"",db[i].g_name,j,"",db[i].size,db[i].time,BLUE,db[i].fname);
	}
	else if(db[i].symbool=='e'){
		int h=n-strlen(db[i].u_name)+1;
		int j=(n1-strlen(db[i].g_name))+1;
		printf("%s%s  %2d %s%*s %s%*s%8d %s %s%s\n",NORMAL_COLOR,db[i].dup,db[i].hard_links,db[i].u_name,h,"",db[i].g_name,j,"",db[i].size,db[i].time,GREEN,db[i].fname);
	
	}
	else{
		int h=n-strlen(db[i].u_name)+1;
		int j=(n1-strlen(db[i].g_name))+1;
		printf("%s%s  %2d %s%*s %s%*s%8d %s %s\n",NORMAL_COLOR,db[i].dup,db[i].hard_links,db[i].u_name,h,"",db[i].g_name,j,"",db[i].size,db[i].time,db[i].fname);
	
	}
}
}
struct st *sort(struct st *db)
{
	struct st temp;
	for(int i=1;i<cnt;i++){
		for(int j=0;j<cnt-1;j++){
		if(strcmp(db[j].fname,db[j+1].fname)>0)
		{
			temp=db[j];
			db[j]=db[j+1];
			db[j+1]=temp;
			
		}
		}
	}
	return db;
}
//this is for user id length 
int length(struct st *db){
int len,count=0;
int high=0;
for(int i=0;i<cnt;i++){
if(strlen(db[i].u_name)>high){
high=strlen(db[i].u_name);
count++;
}
}
return high;
}

//this is for group iod length

int lengthgp(struct st *db){
int len,count=0;
int high=0;
for(int i=0;i<cnt;i++){
if(strlen(db[i].g_name)>high){
high=strlen(db[i].g_name);
count++;
}
}
return high;
}


int main(int n, char *ar[]){
	DIR *d;
	struct stat s;
	char per;
	int k=9;
	char dum[11];
	d=opendir(".");

	struct group *gr;
	struct passwd *ur;
	struct st *db=NULL;
	if(d==NULL){
		printf("error");
	}
	struct dirent *dir;
	while((dir=readdir(d))!=NULL){
		int k=stat(dir->d_name,&s);
		if(dir->d_type==DT_DIR){
			if(dir->d_name[0]=='.')continue;
			db=realloc(db,(cnt+1)*sizeof(struct st));
			strcpy(dum,arr);
			for(int i=0;i<3;i++){//33204//100664
				per=((s.st_mode>>(i*3))&7);
				k=9;
				k=k-(i*3);
				for(int j=0;j<3;j++){
					if((per>>j)&1)
					{
						if(j==0){
							dum[k-j]='x';
						}
						else if(j==1){
							dum[k-j]='w';
						}
						else if(j==2){
							dum[k-j]='r';
						}
					}
				}
			}
			dum[0]='d';
			db[cnt].symbool='d';

		}
		/************for excutable file***/
		else if(check(s.st_mode)){
			db=realloc(db,(cnt+1)*sizeof(struct st));
			strcpy(dum,arr);
			for(int i=0;i<3;i++){//33204//100664
				per=((s.st_mode>>(i*3))&7);
				k=9;
				k=k-(i*3);
				for(int j=0;j<3;j++){
					if((per>>j)&1)
					{
						if(j==0){
							dum[k]='x';
						}
						else if(j==1){
							dum[k-j]='w';
						}
						else if(j==2){
							dum[k-j]='r';
						}
					}
				}
			}
			db[cnt].symbool='e';
		}
		else{
			if(dir->d_name[0]=='.')continue;
			db=realloc(db,(cnt+1)*sizeof(struct st));
			strcpy(dum,arr);
			for(int i=0;i<3;i++){//33204//100664
				per=((s.st_mode>>(i*3))&7);
				k=9;
				k=k-(i*3);
				for(int j=0;j<3;j++){
					if((per>>j)&1)
					{
						if(j==0){
							dum[k-j]='x';
						}
						else if(j==1){
							dum[k-j]='w';
						}
						else if(j==2){
							dum[k-j]='r';
						}
					}
				}
			}
			db[cnt].symbool='g';
		}
			gr=getgrgid(s.st_gid);//it is for group name
			db[cnt].g_name=calloc(1,strlen(gr->gr_name));
			strcpy(db[cnt].g_name,gr->gr_name);
		char time[50];	
			ur=getpwuid(s.st_uid);//it is for user name
			db[cnt].u_name=calloc(1,strlen(ur->pw_name));
			strcpy(db[cnt].u_name,ur->pw_name);
			db[cnt].time=calloc(1,50);	
			//int d_time=s.st_ctim.tv_sec+19800;
			strftime(time,sizeof(time),"%a %d %R",(localtime(&s.st_ctim.tv_sec)));
			strcpy(db[cnt].time,time);
			//printf("%s\n",db[cnt].time);

			int size=strlen(dir->d_name);
			db[cnt].dup=calloc(1,11);
			strcpy(db[cnt].dup,dum);
			db[cnt].fname=calloc(1,size+1);
			strcpy(db[cnt].fname,dir->d_name);
			db[cnt].hard_links=s.st_nlink;
			db[cnt].size=s.st_size;
			cnt++;
		

	}
	int num,num_gp;
	if(db!=NULL){
	db=sort(db);
	num=length(db);
	num_gp=lengthgp(db);
	printf("PERMISSIONS  L UID%*s  GID%*s     SIZE LOM LD HH:MM FNAME\n",num-3,"",num_gp-3,"");
	print(db,num,num_gp);
	
	
	}


}


#include<stdio.h>
int main()
{
	FILE *fp;
	char buf[10];
	int x,y;
	fp=fopen("v.txt","r");
	fscanf(fp,"%d %d",&x,&y);
	fclose(fp);
	int sum=x+y;
	fp=fopen("v.txt","w");
	fprintf(fp,"a=%d",x);
	fprintf(fp,"b=%d",y);
	fprintf(fp," sum=%d",sum);
	fclose(fp);
}
	

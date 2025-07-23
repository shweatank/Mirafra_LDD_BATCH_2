#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#if 0
int main()                    //strcpy(dest,src);
{
		char *str="kavya";
		char *des=malloc(sizeof(str));
		char *temp=des;
		while(*temp++=*str++);
		printf("%s\n",des);
		return 0;
}
#endif

#if 0           //strrev
int main()
{
		//char *str="kavya";
		char str[]="kavya";
		//char str[20];
		//printf("Enter string:\n");
		//scanf("%s",str);
		int j=strlen(str)-1;
		int i=0;
		while(i<j)
		{
				char ch=str[i];
				str[i]=str[j];
				str[j]=ch;
				i++;
				j--;
		}
		printf("%s\n",str);
		return 0;
}
#endif

#if 1                  //string reverse using recursion
void rev(char *,int ,int);
int main()
{
		char str[20];
		printf("Enter string:\n");
		scanf("%s",str);
		int i=0,j=strlen(str)-1;
		rev(str,i,j);
		printf("reversed string:%s\n",str);
        return 0;
}
void rev(char *str,int i,int j)
{
		if(i<j)
		{
				char temp=str[i];
				str[i]=str[j];
				str[j]=temp;
				rev(str,i+1,j-1);
		}
		//return str;
		//printf("\n");
}
#endif

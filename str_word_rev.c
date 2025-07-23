#include<stdio.h>
#include<string.h>
#if 0
void rev(char *l,char *f)
{
		while(l<f)
		{
				char temp=*l;
				*l=*f;
				*f=temp;
				l++;
				f--;
		}
		return;
}
int main()
{
		char str[50];
		printf("Enter string:\n");
		scanf("%[^\n]s",str);
		int j=0;
		int i=0;
		int k=strlen(str);
		//for(int i=0;str[i]!='\0';i++)
		while(i<=k)
		{
				if(str[i]==' ' || str[i]=='\0')
				{
						rev(&str[j],&str[i-1]);
						j=i+1;
				}
				i++;
		}
		printf("%s",str);
		return 0;
}
#endif

#if 1
void rev(char *l, char *f)
{
		while(l<f)
		{
				char temp=*l;
				*l=*f;
				*f=temp;
				l++;
				f--;
		}
		return;
}
void strrev(char *ptr)
{
		int i=0;
		int j=strlen(ptr)-1;
		while(i<j)
		{
				char temp=ptr[i];
				ptr[i]=ptr[j];
				ptr[j]=temp;
				i++;
				j--;
		}
		return;
}
int main()
{
		char str[50];
		printf("Enter string:\n");
		scanf("%[^\n]s",str);
		strrev(str);
		int i=0;
		int j=0;
		int k=strlen(str);
		while(i<=k)
		{
				if(str[i]==' ' || str[i]=='\0')
				{
						rev(&str[j],&str[i-1]);
						j=i+1;
				}
				i++;
		}
		printf("%s\n",str);
		return 0;
}
#endif

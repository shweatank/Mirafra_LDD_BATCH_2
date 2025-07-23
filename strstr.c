#include<stdio.h>
int main()
{
		//char *str="hello world";
		//char *ch="world";
		char str[50];
		char ch[20];
		printf("Enter string\n");
		scanf("%[^\n]s",str);
		printf("Enter sub string\n");
		scanf("%s",ch);
		for(int i=0;str[i]!='\0';i++)
		{
				if(str[i]==ch[0])
				{
						int flag=0;
						for(int j=0;ch[j]!='\0';j++)
						{
								if(str[i+j]!=ch[j])
								{
										flag=1;
										break;
								}
						}
						if(flag==0)
						{
								//return i;
								printf("sub string is present\n");
								break;
						}
				}
		}
		//if(flag==1)
		//printf("sub string is not present\n");
		return 0;
}

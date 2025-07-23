// Online C compiler to run C program online
#include <stdio.h>
#include<stdlib.h>
#if 0
int power(int n)
{
    int d=1;
    if(n==0)
    {
        return 1;
    }
    else
    {
        for(int i=1;i<=n;i++)
        {
            d=d*2;
        }
    }
    return d;
}
int main() {
   char ch[4]="1010";
   int num=0;
   for(int i=3;i>=0;i--)
   {
       if(ch[i]=='1')
       {
           int n=power(4-(i+1));
           //printf("%d\n",n);
           num=num+n;
       }
   }
   printf("%d",num);
    return 0;
}
#endif

#if 1    //binary to decimal
int main()
{
		int num;
		printf("Enter number:\n");
		scanf("%d",&num);
		printf("%d\n",num);
		int dec=0,weight=1,rec;
		while(num>0)
		{
				rec=num % 10;
				dec=dec+rec*weight;
				num=num/10;
				weight=weight*2;
		}
		printf("%d\n",dec);
		return 0;
}
#endif

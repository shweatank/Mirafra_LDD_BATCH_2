#include<stdio.h>
#include<stdlib.h>
int main()
{
		int **arr;
		arr=(int **)malloc(2*sizeof(int *));
		for(int i=0;i<2;i++)
		{
				arr[i]=(int *)malloc(3*sizeof(int));
		}
		printf("Enter elements:\n");
		for(int i=0;i<2;i++)
		{
				for(int j=0;j<3;j++)
				{
						scanf("%d",&arr[i][j]);
						printf("%d ",arr[i][j]);
				}
		}
		//for(int i=0;i<2;i++)    //freeing col ele(malloc(3*sizeof(int)))
		//{
		//		free(arr[i]);
		//}
		//free(arr);        //freeing row elements(malloc(2*sizeof(int *)))
		printf("\n");
		return 0;
}

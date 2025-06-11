/* pyramid pattern 
5
45
345
2345
12345
2345
345
45
5
*/

#include <stdio.h>

int main()
{
   int num;
   printf("Enter a num : ");
   scanf("%d", &num);
    
    for(int i=num;i>0;i--)
    {
        for(int j=i;j<=num;j++)
        {
            printf("%d ", j);
        }
        printf("\n");
        
    }
    for(int i=2;i<=num;i++)
    {
        for(int j=i;j<= num;j++)
        {
        printf("%d ", j);
        }
        printf("\n");
    }
    
   return 0;
}

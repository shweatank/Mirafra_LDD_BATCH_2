/* x pattern value
1  4
 23
 23
1  4
*/

#include <stdio.h>

int main()
{
   int num;
   printf("Enter a num : ");
   scanf("%d", &num);
  int c;
  for(int i=1;i<=num;i++)
  {
      c=0;
      for(int j=1;j<=num;j++)
      {
          if(i == j || i+j == num+1)
          {
              printf("%d ", j);
          }
          else
          {
              printf("_");
          }
      }
      printf("\n");
  }
   return 0;
}

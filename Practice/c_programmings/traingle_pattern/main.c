/* Triangle pattern value
1234
5__6
7_8
9
*/

#include <stdio.h>

int main()
{
   int num;
   printf("Enter a num : ");
   scanf("%d", &num);
   int c=0;
  for(int i=1; i<= num; i++)
  {
      for(int j=1;j<=num;j++)
      {
          if(i==1 || j == 1 || i+j == num+1)
          {
                c=c+1;
                printf("%d ", c);
          }
          else
          {
              printf("  ");
          }
      }
      printf("\n");
  }
   return 0;
}

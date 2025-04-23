#include <stdio.h>

void swap(int *a, int *b){
   int c;
   c = *a;
   *a = *b;
   *b = c;
}

int main(){
   
   void (*ptr)(int *, int *) = swap;
   
   int x = 10, y = 20;
   printf("Values of x: %d and y: %d before swap\n", x, y);
   
   (*ptr)(&x, &y);
   printf("Values of x: %d and y: %d after swap", x, y);

   return 0;

}

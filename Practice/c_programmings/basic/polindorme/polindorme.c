 #include<stdio.h>
#include<stdlib.h>
int main(){
    int n = 12321;
    int temp = n;
    int rev=0;
    while(temp != 0)
    {
        int rem = temp % 10;
        rev = rev * 10 + rem;
        temp /= 10;
    }
    if(n == rev)
    {
        printf("polindorme");
    }
    else
    {
        printf("not");
    }
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct data{
    int a;
    char c;
    char b[20];
};
int main()
{
    struct data d;
    char buffer[50];

    printf("Enter a (int) value: ");
    scanf("%s",buffer);
    d.a = atoi(buffer);

    printf("Enter b (char) value : ");
    scanf(" %s",buffer);
    d.b= buffer[0];

    printf("Enter c (string) value : ");
    scanf(" %[^\n]",buffer);
    strcpy(d.b, buffer);

    printf("Output Data is \n\ta: %d\n\tb: %c\n\tc: %s\n",d.a,d.c,d.b);
    return 0;
}
 


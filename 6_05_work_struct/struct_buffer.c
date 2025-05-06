#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct data{
    int a;
    char b;
    char c[20];
};

void print_data(struct data d)
{
    printf("Output Data is \n\ta: %d\n\tb: %c\n\tc: %s\n",d.a,d.b,d.c);
}
int main()
{
    struct data d;
    char buffer[50];

    printf("Enter a (int) value: ");
    scanf("%s",buffer);
    d.a = atoi(buffer);

    printf("Enter b (char) value : ");
    scanf(" %s",buffer);
    d.b = buffer[0];

    printf("Enter c (string) value : ");
    scanf(" %[^\n]",buffer);
    strcpy(d.c, buffer);

    struct data d2 = d;
    print_data(d2);

    return 0;
}
/*
int main()
{
    struct data d;
    char buffer[100];

    printf("Enter a (int) value: ");
    scanf("%d",&d.a);
    strcpy(buffer,d.a);

    printf("Enter b (char) value : ");
    scanf(" %c",&d.b);
    buffer[strlen(buffer)] = d.b;
    buffer[strlen(buffer)+1] = '\0';


    printf("Enter c (string) value : ");
    scanf(" %[^\n]",&d.c);
    strcat(buffer,d.c);

    struct data d2;
    d2.a = atoi(buffer);
    d2.b = buffer+

    print_data(d2);

    return 0;
}*/

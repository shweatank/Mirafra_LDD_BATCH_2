/******************************************************************************

Welcome to GDB Online.
  GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby, 
  C#, OCaml, VB, Perl, Swift, Prolog, Javascript, Pascal, COBOL, HTML, CSS, JS
  Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
int (*p[])(int,int);
int add(int a,int b){
    return a+b;
}
int sub(int a,int b){
    return a-b;
}
int mult(int a,int b){
    return a*b;
}
int divi(int a,int b){
    return a/b;
}


int main()
{
    int a,b,z;
    char c;
    printf("enter the input like 3 * 2 in format\n");
    scanf("%d %c %d",&a,&c,&b);
    printf("given data=%d %c %d\n",a,c,b);
    int (*p[])(int,int)={&add,&sub,&mult,&divi};
    
    switch(c){
        case '+':
            z=(*p[0])(a,b);
            printf("addition=%d\n",z);
            break;
        case '-':
            z=(*p[1])(a,b);
            printf("substraction=%d\n",z);
            break;
        case '*':
            z=(*p[2])(a,b);
            printf("multiplication=%d\n",z);
            break;
        case '/':
            z=(*p[3])(a,b);
            printf("division=%d",z);
            break;
        default:
        printf("inavlid opearation\n");
    }
    return 0;
}

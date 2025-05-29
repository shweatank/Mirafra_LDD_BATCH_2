#include<stdio.h>

int display(int(* ptr)(int,int));
int add(int,int);

int main(){
	display(add);
	return 0;
}

int display(int(*ptr)(int a,int b)){
	(*ptr)(3,2);
}
int add(int a,int b){
	printf("%d",a+b);
}

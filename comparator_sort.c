#include<stdio.h>
#include<stdlib.h>


//int asc_sort(void* arr,int t_size,int e_size,int (*ptr)(const void*,const void*)){

int comp_asc(const void* a,const void* b){
	return (*(int*)a-*(int*)b);
}
int comp_dec(const void* a,const void* b){
	return (*(int*)b-*(int*)a);
}
int comp_even_first(const void* a,const void* b){
	int* x=a;
	int* y=b;

	if((*x%2==0) && (*y%2!=0))
		return -1;
	if((*x%2!=0) && (*y%2==0))
		return 1;
	return *x-*y;
}


int main(){

	int arr[]={3,1,6,4,9,2};
	int size=6;

	qsort(arr,size,sizeof(int),comp_asc);
	printf("array in ascending order\n");
	for(int i=0;i<size;i++){
		printf("%d\t",arr[i]);
	}
	printf("\n");

	qsort(arr,size,sizeof(int),comp_dec);
	printf("array in decending order\n");
	for(int i=0;i<size;i++){
		printf("%d\t",arr[i]);
	}
	printf("\n");
	
	qsort(arr,size,sizeof(int),comp_even_first);
	printf("array in even first order\n");
	for(int i=0;i<size;i++){
		printf("%d\t",arr[i]);
	}
	printf("\n");
	return 0;
}

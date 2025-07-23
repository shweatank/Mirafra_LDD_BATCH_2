#include<stdio.h>


void separateEvnOdd(int array[],int size){

	int left = 0;
	int right = size -1;

	while(left < right){

		while(array[left] %2 == 0 && left < right){
			left++;
		}
		while(array[right] %2 == 1 && left < right){
			right--;
		}

		if(left < right){
			int temp = array[left];
			array[left] = array[right];
			array[right] = temp;
		}

	}
}

int main(){

	int array[] = {1,2,3,4,5,6,7,8};
	int size  =  sizeof(array)/sizeof(array[0]);

	separateEvnOdd(array,size);
	for(int i =0; i < size ; i++){
		printf("%d ",array[i]);
	}
	return 0;
}

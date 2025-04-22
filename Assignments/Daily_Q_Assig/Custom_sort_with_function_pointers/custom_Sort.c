#include <stdio.h>

typedef int (*CompareFunc)(int, int);

void sort_array(int arr[], int size, CompareFunc compare) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (compare(arr[j], arr[j + 1]) > 0) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
int asc(int a, int b) {
return a - b;
}

int desc(int a, int b) {
return b - a;
}

int even_first(int a, int b) {
if (a % 2 == 0 && b % 2 != 0) 
return -1;
if (a % 2 != 0 && b % 2 == 0) 
return 1;
return a - b;
}

int odd_first(int a, int b) {
if (a % 2 != 0 && b % 2 == 0) 
return -1;
if (a % 2 == 0 && b % 2 != 0) 
return 1; 
return a - b;
}

void print_array(int arr[], int size) {
for (int i = 0; i < size; i++) {
    printf("%d ", arr[i]);
}
printf("\n");
}

int main() {
int numbers[] = {7, 2, 10, 3, 8, 1, 9, 4};
int size = 8;

printf("Original: ");
print_array(numbers, size);

int asc_numbers[8];
int descc_numbers[8];
int even_numbers[8];
int odd_numbers[8];
for (int i = 0; i < size; i++) {
    asc_numbers[i] = descc_numbers[i] = even_numbers[i] = numbers[i];
}

for (int i = 0; i < size; i++) {
    odd_numbers[i] = numbers[i];
}
sort_array(asc_numbers, size, asc);
printf("Asc: ");
print_array(asc_numbers, size);

sort_array(descc_numbers,size, desc);
printf("des: ");
print_array(descc_numbers, size);

sort_array(even_numbers,size, even_first);
printf("Even first: ");
print_array(even_numbers,size);

sort_array(odd_numbers, size, odd_first);
printf("odd first: ");
print_array(odd_numbers,size);
return 0;
}

#include <stdio.h>
#include <stdlib.h>

void *my_malloc(size_t size) {
    return malloc(size);
}

void *my_calloc(size_t count, size_t size) {
    return calloc(count, size);
}

void *my_realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

void allocate_and_print(void *(*alloc_func)(size_t), size_t size) {
    int *arr = (int *) alloc_func(size * sizeof(int));
    for (int i = 0; i < size; ++i) {
        arr[i] = i * 10;
        printf("%d ", arr[i]);
    }
    free(arr);
}

int main() {
    void *(*allocators[])(size_t) = {my_malloc, my_calloc};
    size_t size = 5;

    printf("using malloc:\n");
    allocate_and_print(allocators[0], size);

    printf("\nusing calloc:\n");
    allocate_and_print(allocators[1], size);

    return 0;
}


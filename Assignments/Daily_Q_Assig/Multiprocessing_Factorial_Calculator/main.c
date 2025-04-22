#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

unsigned long long factorial(int num) {
    if (num < 0) return 0;
    unsigned long long res = 1;
    for (int i = 2; i <= num; ++i)
        res *= i;
    return res;
}

int main() {
    int numbers[] = {3, 5, 7, 10};
    int N = sizeof(numbers) / sizeof(numbers[0]);

    unsigned long long *results = mmap(NULL, N * sizeof(unsigned long long),PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (results == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    for (int i = 0; i < N; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            return 1;
        } else if (pid == 0) {
            results[i] = factorial(numbers[i]);
            exit(0);
        }
    }

    for (int i = 0; i < N; ++i) {
        wait(NULL);
    }

    printf("factorials:\n");
    for (int i = 0; i < N; ++i) {
        printf("%d! = %llu\n", numbers[i], results[i]);
    }

    munmap(results, N * sizeof(unsigned long long));
    return 0;
}


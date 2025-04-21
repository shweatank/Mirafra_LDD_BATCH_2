#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

// Compute factorial using a loop
unsigned long long compute_factorial(int value) {
    if (value < 0) return 0;
    unsigned long long result = 1;
    for (int i = 2; i <= value; i++) {
        result *= i;
    }
    return result;
}

int main() {
    int input_values[] = {3, 5, 7, 10};
    int count = sizeof(input_values) / sizeof(input_values[0]);

    // Create shared memory for results
    unsigned long long *shared_results = mmap(NULL, count * sizeof(unsigned long long),
                                              PROT_READ | PROT_WRITE,
                                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_results == MAP_FAILED) {
        perror("Memory mapping failed");
        return 1;
    }

    for (int idx = 0; idx < count; idx++) {
        pid_t child_pid = fork();

        if (child_pid < 0) {
            perror("Forking process failed");
            return 1;
        } else if (child_pid == 0) {
            // Child process computes and stores factorial
            shared_results[idx] = compute_factorial(input_values[idx]);
            exit(0);
        }
    }

    // Wait for all child processes
    for (int j = 0; j < count; j++) {
        wait(NULL);
    }

    // Display results
    printf("factorials:\n");
    for (int k = 0; k < count; k++) {
        printf("%d! = %llu\n", input_values[k], shared_results[k]);
    }

    munmap(shared_results, count * sizeof(unsigned long long));
    return 0;
}


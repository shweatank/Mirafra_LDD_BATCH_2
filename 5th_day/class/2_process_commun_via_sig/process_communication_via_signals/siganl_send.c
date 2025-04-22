#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    pid_t pid;
    int choice;

    printf("Enter PID to send signal to: ");
    scanf("%d", &pid);

    printf("Choose operation:\n");
    printf("1. Add\n");
    printf("2. Subtract\n");
    printf("3. Multiply\n");
    printf("4. Divide\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            kill(pid, SIGUSR1);
            break;
        case 2:
            kill(pid, SIGUSR2);
            break;
        case 3:
            kill(pid, SIGINT);
            break;
        case 4:
            kill(pid, SIGTERM);
            break;
        default:
            printf("Invalid choice.\n");
            return 1;
    }

    printf("Signal sent to PID %d\n", pid);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int a = 20, b = 5;

void handle_add(int sig) {
    printf("ADD: %d + %d = %d\n", a, b, a + b);
}

void handle_sub(int sig) {
    printf("SUB: %d - %d = %d\n", a, b, a - b);
}

void handle_mul(int sig) {
    printf("MUL: %d * %d = %d\n", a, b, a * b);
}

void handle_div(int sig) {
    if (b != 0)
        printf("DIV: %d / %d = %d\n", a, b, a / b);
    else
        printf("DIV: Division by zero\n");
}

int main() {
    signal(SIGUSR1, handle_add);
    signal(SIGUSR2, handle_sub);
    signal(SIGINT, handle_mul);
    signal(SIGTERM, handle_div);

    printf("Receiver PID: %d\n", getpid());
    while (1) {
        pause();
    }
    return 0;
}
